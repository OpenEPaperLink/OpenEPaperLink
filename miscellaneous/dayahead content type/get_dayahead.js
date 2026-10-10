/**
 * get_dayahead - day-ahead prijzen als JSON.
 *
 * Bronnen:
 *  - Nord Pool is de primaire bron voor de zones die het kent: snel (een paar
 *    honderd ms), in kwartieren, en direct in NOK/SEK/DKK. ENTSO-E publiceert
 *    dezelfde veilinguitkomst later opnieuw en mist soms een hele dag.
 *  - ENTSO-E is de fallback: voor zones die Nord Pool niet kent (CH, CZ, ES,
 *    PT, RO, SI, SK) en wanneer Nord Pool faalt of vandaag niet volledig heeft.
 *  - Parameter bron=nordpool of bron=entsoe dwingt een bron af, om te testen.
 *    Zo'n aanroep schrijft de gedeelde cache niet.
 *
 * Cachegedrag:
 *  - Het vervallen hangt niet aan de TTL maar aan een dekkingstest op de data
 *    zelf: reikt de laatste tijdstempel tot het eind van de dag die nu nodig is.
 *    Daardoor volstaan ongeveer vier fetches per dag in plaats van 24.
 *  - Altijd vereist: dekking tot het eind van vandaag. Die eis valt vanzelf om
 *    na lokale middernacht, dus daar is geen schema voor nodig.
 *  - Tussen 13:00 en 17:00 aanvullend vereist: dekking tot het eind van morgen,
 *    want SDAC publiceert rond 13:00. Na 17:00 vervalt die eis weer.
 *  - Een poging-sleutel met TTL 1 uur remt herpogingen zolang morgen nog niet
 *    gepubliceerd is, zodat niet elke request 26 seconden gaat wachten.
 *  - Wie de lock niet krijgt terwijl een andere aanroep aan het ophalen is,
 *    krijgt direct de oude set. Alleen bij een koude start wordt gewacht.
 */

// Secrets horen in Projectinstellingen > Scripteigenschappen.
function prop_(name, fallback) {
  var v = PropertiesService.getScriptProperties().getProperty(name);
  return v || fallback;
}

var CACHE_SAFE_BYTES = 90 * 1024; // CacheService weigert waarden boven 100 KB
var LOG_BODY_CHARS = 500;         // hoeveel van een foutbody we loggen
var TTL_OUTPUT = 21600;           // 6 uur, het maximum van CacheService
var TTL_LASTGOOD = 21600;
var TTL_XML = 3600;
var TTL_CURRENCY = 21600;
var TTL_ATTEMPT = 3600;           // rem op herpogingen binnen het publicatievenster
var TTL_GAP_NORDPOOL = 600;       // rem op herpogingen bij een gat, Nord Pool is snel
var TTL_GAP_ENTSOE = 3600;        // idem voor zones die alleen ENTSO-E hebben
var LOCK_QUICK_MS = 200;          // korte poging; daarna liever oude data serveren
var LOCK_COLD_MS = 30000;         // koude start: geen oude data, dus wel wachten
var PUBLISH_FROM_HOUR = 13;       // SDAC publiceert morgen rond 13:00
var PUBLISH_UNTIL_HOUR = 17;
var DEFAULT_COUNTRY = 'NL';

/* ------------------------------------------------------------------ */
/* Zones                                                               */
/* ------------------------------------------------------------------ */

// https://transparency.entsoe.eu/content/static_content/Static%20content/web%20api/Guide.html#_areas
// LU deelt de biedzone DE-LU (10Y1001A1001A82H) sinds de DE-AT-LU splitsing
// van oktober 2018. Nord Pool noemt die zone GER.
// nordpool: deliveryArea bij Nord Pool, null als Nord Pool de zone niet kent.
var ZONES = {
  'AT':  { domain: '10YAT-APG------L', currency: null, nordpool: 'AT' },
  'BE':  { domain: '10YBE----------2', currency: null, nordpool: 'BE' },
  'CH':  { domain: '10YCH-SWISSGRIDZ', currency: null, nordpool: null },
  'CZ':  { domain: '10YCZ-CEPS-----N', currency: null, nordpool: null },
  'DE':  { domain: '10Y1001A1001A82H', currency: null, nordpool: 'GER' },
  'LU':  { domain: '10Y1001A1001A82H', currency: null, nordpool: 'GER' },
  'DK1': { domain: '10YDK-1--------W', currency: 'DKK', nordpool: 'DK1' },
  'DK2': { domain: '10YDK-2--------M', currency: 'DKK', nordpool: 'DK2' },
  'EE':  { domain: '10Y1001A1001A39I', currency: null, nordpool: 'EE' },
  'ES':  { domain: '10YES-REE------0', currency: null, nordpool: null },
  'FI':  { domain: '10YFI-1--------U', currency: null, nordpool: 'FI' },
  'FR':  { domain: '10YFR-RTE------C', currency: null, nordpool: 'FR' },
  'LT':  { domain: '10YLT-1001A0008Q', currency: null, nordpool: 'LT' },
  'LV':  { domain: '10YLV-1001A00074', currency: null, nordpool: 'LV' },
  'NL':  { domain: '10YNL----------L', currency: null, nordpool: 'NL' },
  'NO1': { domain: '10YNO-1--------2', currency: 'NOK', nordpool: 'NO1' },
  'NO2': { domain: '10YNO-2--------T', currency: 'NOK', nordpool: 'NO2' },
  'NO3': { domain: '10YNO-3--------J', currency: 'NOK', nordpool: 'NO3' },
  'NO4': { domain: '10YNO-4--------9', currency: 'NOK', nordpool: 'NO4' },
  'NO5': { domain: '10Y1001A1001A48H', currency: 'NOK', nordpool: 'NO5' },
  'PL':  { domain: '10YPL-AREA-----S', currency: null, nordpool: 'PL' },
  'PT':  { domain: '10YPT-REN------W', currency: null, nordpool: null },
  'RO':  { domain: '10YRO-TEL------P', currency: null, nordpool: null },
  'SE1': { domain: '10Y1001A1001A44P', currency: 'SEK', nordpool: 'SE1' },
  'SE2': { domain: '10Y1001A1001A45N', currency: 'SEK', nordpool: 'SE2' },
  'SE3': { domain: '10Y1001A1001A46L', currency: 'SEK', nordpool: 'SE3' },
  'SE4': { domain: '10Y1001A1001A47J', currency: 'SEK', nordpool: 'SE4' },
  'SI':  { domain: '10YSI-ELES-----O', currency: null, nordpool: null },
  'SK':  { domain: '10YSK-SEPS-----K', currency: null, nordpool: null }
};

function lookupZone_(country) {
  var zone = ZONES[country];
  if (!zone) {
    throw new Error('Onbekende country "' + country + '". Geldig: ' +
      Object.keys(ZONES).join(', '));
  }
  return zone;
}

/* ------------------------------------------------------------------ */
/* Entry point                                                         */
/* ------------------------------------------------------------------ */

function doGet(e) {
  var ctx = newContext_(e);
  ctx.log('binnengekomen', {
    parameter: ctx.e.parameter,
    queryString: ctx.e.queryString || null,
    pathInfo: ctx.e.pathInfo || null
  });

  try {
    var jsonData = buildDayAhead_(ctx);
    var body = JSON.stringify(jsonData);

    // Alleen wegschrijven wanneer de data daadwerkelijk vers is opgehaald.
    // Schrijven na een cache-hit verlengt de TTL en bevriest de cache voorgoed.
    ctx.step = 'cache-write';
    if (ctx.fromCache) {
      ctx.log('cache-write overgeslagen, data kwam uit de cache');
    } else if (ctx.bron) {
      // Een afgedwongen bron is een test; die mag de gedeelde set niet vervangen.
      ctx.log('cache-write overgeslagen, bron afgedwongen (' + ctx.bron + ')');
    } else {
      safePut_(ctx, 'output' + ctx.country, body, TTL_OUTPUT);
      safePut_(ctx, 'lastgood_' + ctx.country, body, TTL_LASTGOOD);
    }

    ctx.log('gereed', { bron: ctx.source, punten: jsonData.length, bytes: body.length, ms: ctx.elapsed() });
    return reply_(ctx, body, jsonData);

  } catch (err) {
    // serveStale is geen storing maar een bewuste keuze: een andere aanroep is
    // al aan het ophalen, dus zelf wachten heeft geen zin.
    if (err && err.serveStale) {
      ctx.log('geen eigen fetch: ' + err.message);
    } else {
      ctx.fail(err);
    }

    ctx.step = 'fallback';
    var stale = safeGet_(ctx, 'lastgood_' + ctx.country);
    if (stale && !ctx.debug) {
      // Bewust niet herschrijven: de fallback mag zichzelf niet in leven houden.
      ctx.log('val terug op laatst bekende goede data', { bytes: stale.length });
      return ContentService.createTextOutput(stale)
        .setMimeType(ContentService.MimeType.JSON);
    }
    return replyError_(ctx, err);
  }
}

/* ------------------------------------------------------------------ */
/* Hoofdlogica                                                         */
/* ------------------------------------------------------------------ */

function buildDayAhead_(ctx) {
  ctx.step = 'zone';
  var zone = lookupZone_(ctx.country);
  ctx.domain = zone.domain;
  ctx.currency = zone.currency;
  ctx.nordpool = zone.nordpool;
  ctx.log('zone bepaald', { country: ctx.country, domain: ctx.domain, currency: ctx.currency });

  ctx.step = 'cache-read';
  var cached = readCachedSet_(ctx);
  if (cached) {
    var verdict = cacheVerdict_(ctx, cached);
    if (verdict.bruikbaar) {
      ctx.fromCache = true;
      ctx.log('cache-hit op output' + ctx.country + ': ' + verdict.reden, verdict.data);
      return cached;
    }
    ctx.log('cache niet meer bruikbaar: ' + verdict.reden, verdict.data);
  }

  // Markeer de poging vóór de fetch, zodat ook een mislukte poging remt.
  safePut_(ctx, 'poging_' + ctx.country, String(Date.now()), TTL_ATTEMPT);

  if (zone.nordpool && ctx.bron !== 'entsoe') {
    ctx.step = 'nordpool-fetch';
    var npData = null;
    try {
      npData = fetchNordPool_(ctx, zone);
    } catch (err) {
      console.error('[' + ctx.id + '] Nord Pool mislukt: ' + (err && err.stack ? err.stack : err));
      ctx.log('Nord Pool mislukt', { fout: String((err && err.message) || err) });
      if (ctx.bron === 'nordpool') throw err;
    }
    if (npData) {
      var npOordeel = completeToday_(npData);
      if (npOordeel.compleet || ctx.bron === 'nordpool') {
        ctx.source = 'nordpool';
        ctx.log('Nord Pool gebruikt: ' + npOordeel.reden);
        return npData;
      }
      ctx.log('Nord Pool onvolledig, val terug op ENTSO-E: ' + npOordeel.reden);
    }
  } else if (!zone.nordpool) {
    ctx.log('zone niet bij Nord Pool, ENTSO-E gebruikt');
  }

  ctx.step = 'wisselkoers';
  var factor = currencyFactor_(ctx);

  ctx.step = 'entsoe-fetch';
  ctx.source = 'entsoe';
  var xmlContent = fetchEntsoe_(ctx);

  ctx.step = 'parse';
  var root = parseEntsoe_(ctx, xmlContent);

  ctx.step = 'extract';
  return extractPoints_(ctx, root, factor);
}

function readCachedSet_(ctx) {
  if (ctx.noCache) {
    ctx.log('cache-read overgeslagen op verzoek (nocache=1)');
    return null;
  }
  var raw = safeGet_(ctx, 'output' + ctx.country);
  if (raw == null) {
    ctx.log('cache-miss op output' + ctx.country);
    return null;
  }
  try {
    return JSON.parse(raw);
  } catch (err) {
    ctx.log('cache bevatte onleesbare JSON, genegeerd',
      { bytes: raw.length, fout: String(err.message || err) });
    return null;
  }
}

/**
 * Bepaalt of de opgeslagen set nog volstaat. De test kijkt naar de laatste
 * tijdstempel en niet naar het aantal punten, zodat dagen van 23 en 25 uur
 * vanzelf goed gaan.
 */
function cacheVerdict_(ctx, jsonData) {
  var eindVandaag = localMidnightEpoch_(1);
  var eindMorgen = localMidnightEpoch_(2);
  var uur = new Date().getHours();
  var inPublicatievenster = (uur >= PUBLISH_FROM_HOUR && uur < PUBLISH_UNTIL_HOUR);

  var laatste = jsonData.length ? jsonData[jsonData.length - 1].time : null;
  var data = {
    punten: jsonData.length,
    laatste: laatste === null ? null : new Date(laatste * 1000).toISOString(),
    uur: uur,
    publicatievenster: inPublicatievenster
  };

  if (!coversThrough_(jsonData, eindVandaag)) {
    return { bruikbaar: false, reden: 'dekt vandaag niet meer', data: data };
  }

  // Een set kan tot het eind van vandaag reiken en toch een gat hebben: op
  // 10 oktober 2026 stuurde ENTSO-E voor NL vrijdag en zondag, maar geen
  // zaterdag. Zo'n set niet tot middernacht blijven serveren.
  var gat = firstGapFrom_(jsonData, localMidnightEpoch_(0));
  if (gat !== null) {
    data.gat = new Date(gat * 1000).toISOString();
    // Eigen remsleutel, los van de poging_ sleutel van het publicatievenster:
    // anders houdt elke eerdere fetch het herstel tot een uur tegen.
    var pogingGat = safeGet_(ctx, 'gatpoging_' + ctx.country);
    if (pogingGat) {
      data.laatstePoging = new Date(Number(pogingGat)).toISOString();
      return { bruikbaar: true, reden: 'gat in vandaag, herpoging afgeremd', data: data };
    }
    safePut_(ctx, 'gatpoging_' + ctx.country, String(Date.now()),
      ctx.nordpool ? TTL_GAP_NORDPOOL : TTL_GAP_ENTSOE);
    return { bruikbaar: false, reden: 'gat in de data van vandaag', data: data };
  }

  if (inPublicatievenster && !coversThrough_(jsonData, eindMorgen)) {
    var poging = safeGet_(ctx, 'poging_' + ctx.country);
    if (poging) {
      data.laatstePoging = new Date(Number(poging)).toISOString();
      return { bruikbaar: true, reden: 'morgen ontbreekt, herpoging afgeremd', data: data };
    }
    return { bruikbaar: false, reden: 'morgen ontbreekt binnen publicatievenster', data: data };
  }

  return { bruikbaar: true, reden: 'dekking voldoende', data: data };
}

/**
 * Reikt de reeks tot aan de opgegeven grens. De laatste tijdstempel is het
 * begin van het laatste interval, vandaar de marge van een uur.
 */
function coversThrough_(jsonData, boundaryEpoch) {
  if (!jsonData || !jsonData.length) return false;
  var laatste = jsonData[jsonData.length - 1].time;
  return (laatste + 3600) >= boundaryEpoch;
}

/**
 * Begin van het eerste gat vanaf `fromEpoch`, of null. Een gat is een stap die
 * groter is dan de kleinste stap in de reeks, of een reeks die pas na
 * `fromEpoch` begint.
 */
function firstGapFrom_(jsonData, fromEpoch) {
  if (!jsonData || !jsonData.length) return fromEpoch;
  if (jsonData[0].time > fromEpoch) return fromEpoch;

  var minStep = null;
  for (var i = 1; i < jsonData.length; i++) {
    var d = jsonData[i].time - jsonData[i - 1].time;
    if (d > 0 && (minStep === null || d < minStep)) minStep = d;
  }
  if (minStep === null) return null;

  for (var j = 1; j < jsonData.length; j++) {
    if (jsonData[j].time <= fromEpoch) continue;
    if (jsonData[j].time - jsonData[j - 1].time > minStep) return jsonData[j - 1].time + minStep;
  }
  return null;
}

/** Heeft de set vandaag volledig, zonder gat en tot het eind van de dag. */
function completeToday_(jsonData) {
  if (!coversThrough_(jsonData, localMidnightEpoch_(1))) {
    return { compleet: false, reden: 'vandaag niet tot het eind' };
  }
  var gat = firstGapFrom_(jsonData, localMidnightEpoch_(0));
  if (gat !== null) {
    return { compleet: false, reden: 'gat vanaf ' + new Date(gat * 1000).toISOString() };
  }
  return { compleet: true, reden: 'vandaag volledig' };
}

/** Lokale middernacht als epoch in seconden, in de tijdzone van het script. */
function localMidnightEpoch_(offsetDays) {
  var now = new Date();
  return new Date(now.getFullYear(), now.getMonth(), now.getDate() + offsetDays,
    0, 0, 0, 0).getTime() / 1000;
}

function staleSignal_(bericht) {
  var err = new Error(bericht);
  err.serveStale = true;
  return err;
}

function currencyFactor_(ctx) {
  var code = ctx.currency;
  if (!code) return 1;

  var key = 'currency' + code;
  var cachedFactor = safeGet_(ctx, key);
  if (cachedFactor != null && !ctx.noCache) {
    ctx.log('wisselkoers uit cache', { code: code, factor: cachedFactor });
    return parseFloat(cachedFactor);
  }

  var t = Date.now();
  var factor;
  try {
    ExchangeApp.setRatesEndpoint(
      'https://openexchangerates.org/api/latest.json?app_id=' + prop_('OXR_APP_ID', 'xxx'),
      3600);
    factor = ExchangeApp.convert(1, 'EUR', code);
  } catch (err) {
    // Wisselkoers is niet fataal: log het en reken in EUR verder.
    console.error('[' + ctx.id + '] wisselkoers ' + code + ' mislukt na ' + (Date.now() - t) +
      ' ms: ' + (err && err.stack ? err.stack : err));
    ctx.log('wisselkoers mislukt, factor 1 gebruikt (prijzen dus in EUR)', { code: code });
    return 1;
  }

  if (!(factor > 0)) {
    ctx.log('WAARSCHUWING: ongeldige wisselkoers, factor 1 gebruikt', { code: code, factor: factor });
    return 1;
  }

  ctx.log('wisselkoers opgehaald', { code: code, factor: factor, ms: Date.now() - t });
  safePut_(ctx, key, String(factor), TTL_CURRENCY);
  return factor;
}

/**
 * Gisteren, vandaag en morgen van Nord Pool, in de valuta van de zone. Nord
 * Pool levert direct in NOK/SEK/DKK, dus geen wisselkoers nodig. Een dag die
 * nog niet gepubliceerd is geeft HTTP 204. Gooit een fout als vandaag niet
 * opgehaald kan worden, zodat de aanroeper op ENTSO-E terugvalt.
 */
function fetchNordPool_(ctx, zone) {
  var tz = Session.getScriptTimeZone();
  var currency = zone.currency || 'EUR';
  var now = new Date();
  var dagen = [-1, 0, 1].map(function (k) {
    // 12:00 als ijkpunt, zodat een zomertijdwissel de datum niet verschuift.
    var d = new Date(now.getFullYear(), now.getMonth(), now.getDate() + k, 12, 0, 0, 0);
    return Utilities.formatDate(d, tz, 'yyyy-MM-dd');
  });

  var requests = dagen.map(function (dag) {
    return {
      url: 'https://dataportal-api.nordpoolgroup.com/api/DayAheadPrices'
        + '?date=' + dag
        + '&market=DayAhead'
        + '&deliveryArea=' + zone.nordpool
        + '&currency=' + currency,
      muteHttpExceptions: true,
      followRedirects: true,
      validateHttpsCertificates: true
    };
  });

  var t = Date.now();
  var responses = UrlFetchApp.fetchAll(requests);
  ctx.fetchMs = Date.now() - t;

  var byTime = {};
  for (var i = 0; i < responses.length; i++) {
    var dag = dagen[i];
    var code = responses[i].getResponseCode();

    if (code === 204) {
      ctx.log('Nord Pool: ' + dag + ' nog niet gepubliceerd');
      if (i === 1) throw new Error('Nord Pool heeft vandaag (' + dag + ') niet');
      continue;
    }
    if (code !== 200) {
      var body = responses[i].getContentText();
      console.error('[' + ctx.id + '] Nord Pool HTTP ' + code + ' voor ' + dag + ': ' +
        body.substring(0, LOG_BODY_CHARS));
      if (i === 1) throw new Error('Nord Pool gaf HTTP ' + code + ' voor vandaag');
      ctx.log('Nord Pool: ' + dag + ' overgeslagen', { httpCode: code });
      continue;
    }

    var json = JSON.parse(responses[i].getContentText());
    var entries = json.multiAreaEntries || [];
    var aantal = 0;
    for (var j = 0; j < entries.length; j++) {
      var price = entries[j].entryPerArea ? entries[j].entryPerArea[zone.nordpool] : null;
      var start = new Date(entries[j].deliveryStart).getTime();
      if (typeof price !== 'number' || isNaN(start)) continue;
      byTime[start / 1000] = price;
      aantal++;
    }
    ctx.log('Nord Pool: ' + dag, {
      punten: aantal,
      status: json.areaStates && json.areaStates.length ? json.areaStates[0].state : null,
      versie: json.version,
      valuta: json.currency
    });
  }

  var times = Object.keys(byTime).sort(function (a, b) { return a - b; });
  var jsonData = [];
  for (var m = 0; m < times.length; m++) {
    jsonData.push({ time: Number(times[m]), price: byTime[times[m]] });
  }

  ctx.log('Nord Pool punten verzameld', {
    aantal: jsonData.length,
    ms: ctx.fetchMs,
    eerste: jsonData.length ? new Date(jsonData[0].time * 1000).toISOString() : null,
    laatste: jsonData.length ? new Date(jsonData[jsonData.length - 1].time * 1000).toISOString() : null
  });

  if (jsonData.length === 0) throw new Error('Nord Pool gaf geen bruikbare punten');
  return jsonData;
}

function fetchEntsoe_(ctx) {
  ctx.periodStart = getFormattedMidnightUTC(-1);
  ctx.periodEnd = getFormattedMidnightUTC(2);
  ctx.xmlUrl = 'https://web-api.tp.entsoe.eu/api'
    + '?documentType=A44'
    + '&out_Domain=' + ctx.domain
    + '&in_Domain=' + ctx.domain
    + '&periodStart=' + ctx.periodStart
    + '&periodEnd=' + ctx.periodEnd
    + '&securityToken=' + prop_('ENTSOE_TOKEN', 'xxx');

  // Cachesleutel zonder token, en kort genoeg voor de 250-tekens limiet.
  var xmlKey = xmlCacheKey_(ctx.domain, ctx.periodStart, ctx.periodEnd);
  ctx.log('venster', {
    periodStart: ctx.periodStart,
    periodEnd: ctx.periodEnd,
    scriptTz: Session.getScriptTimeZone(),
    nu: new Date().toISOString(),
    url: ctx.xmlUrl.replace(/securityToken=[^&]*/, 'securityToken=***')
  });

  if (!ctx.noCache) {
    var cachedXml = safeGet_(ctx, xmlKey);
    if (cachedXml) {
      ctx.log('XML uit cache', { bytes: cachedXml.length });
      return cachedXml;
    }
  }

  var lock = LockService.getScriptLock();
  var gotLock = false;
  var lockWait = Date.now();
  try {
    // Korte poging. Haalt een andere aanroep de data al op, dan is wachten
    // zinloos: de oude set is dan beter dan 26 seconden vertraging.
    gotLock = lock.tryLock(LOCK_QUICK_MS);
    if (!gotLock) {
      var stale = safeGet_(ctx, 'lastgood_' + ctx.country);
      if (stale && !ctx.debug) {
        throw staleSignal_('lock bezet, andere aanroep haalt de data al op');
      }
      ctx.log('lock bezet en geen oude set beschikbaar, alsnog wachten',
        { maxWachtMs: LOCK_COLD_MS });
      gotLock = lock.tryLock(LOCK_COLD_MS);
    }
    ctx.log(gotLock ? 'lock verkregen' : 'lock NIET verkregen, toch doorgegaan',
      { wachtMs: Date.now() - lockWait });

    if (gotLock && !ctx.noCache) {
      var again = safeGet_(ctx, xmlKey);
      if (again) {
        ctx.log('XML inmiddels door andere aanroep gecached', { bytes: again.length });
        return again;
      }
    }

    var t = Date.now();
    var response = UrlFetchApp.fetch(ctx.xmlUrl, {
      muteHttpExceptions: true,
      followRedirects: true,
      validateHttpsCertificates: true
    });
    var code = response.getResponseCode();
    var xmlContent = response.getContentText();
    ctx.fetchMs = Date.now() - t;
    ctx.log('ENTSO-E antwoord', {
      httpCode: code,
      bytes: xmlContent.length,
      ms: ctx.fetchMs,
      contentType: response.getHeaders()['Content-Type'] || null
    });

    if (code !== 200) {
      console.error('[' + ctx.id + '] ENTSO-E HTTP ' + code + ' body: ' +
        xmlContent.substring(0, LOG_BODY_CHARS));
      throw new Error('ENTSO-E gaf HTTP ' + code + ': ' + xmlContent.substring(0, 200));
    }

    safePut_(ctx, xmlKey, xmlContent, TTL_XML);
    return xmlContent;

  } finally {
    if (gotLock) lock.releaseLock();
  }
}

function parseEntsoe_(ctx, xmlContent) {
  var cleaned = xmlContent
    .replace(/xmlns(:[a-z0-9]+)?=["'][^"']*["']/g, '')
    .replace(/<([a-z0-9]+):/g, '<')
    .replace(/<\/([a-z0-9]+):/g, '</');

  var doc;
  try {
    doc = XmlService.parse(cleaned);
  } catch (err) {
    console.error('[' + ctx.id + '] XML parse mislukt. Eerste ' + LOG_BODY_CHARS +
      ' tekens van de opgeschoonde inhoud:\n' + cleaned.substring(0, LOG_BODY_CHARS));
    throw new Error('XML parse mislukt: ' + err.message);
  }

  var root = doc.getRootElement();
  ctx.log('XML geparsed', { root: root.getName() });

  // ENTSO-E stuurt fouten vaak met HTTP 200 en dit documenttype.
  if (root.getName() === 'Acknowledgement_MarketDocument') {
    var reason = root.getChild('Reason');
    var rc = reason ? reason.getChildText('code') : null;
    var rt = reason ? reason.getChildText('text') : null;
    console.error('[' + ctx.id + '] ENTSO-E Acknowledgement: code=' + rc + ' text=' + rt);
    throw new Error('ENTSO-E weigerde de query (reason ' + rc + '): ' + rt);
  }

  return root;
}

/* ------------------------------------------------------------------ */
/* Punten uitlezen                                                     */
/* ------------------------------------------------------------------ */

function resolutionSeconds_(resolution) {
  switch (resolution) {
    case 'PT15M': return 900;
    case 'PT30M': return 1800;
    case 'PT60M': return 3600;
    case 'PT1H':  return 3600;
    case 'P1D':   return 86400;
    default:      return null;
  }
}

function extractPoints_(ctx, root, factor) {
  var timeSeriesList = getDescendantsByTagName(root, 'TimeSeries');
  ctx.log('TimeSeries gevonden', { aantal: timeSeriesList.length });

  if (timeSeriesList.length === 0) {
    throw new Error('Geen TimeSeries in het antwoord voor ' + ctx.country + ' (' + ctx.domain + ')');
  }

  var jobs = [];

  for (var j = 0; j < timeSeriesList.length; j++) {
    var ts = timeSeriesList[j];
    var curveType = ts.getChildText('curveType') || 'A01';
    var periods = ts.getChildren('Period');

    if (periods.length === 0) {
      ctx.log('WAARSCHUWING: TimeSeries zonder Period', { ts: j + 1 });
      continue;
    }

    for (var p = 0; p < periods.length; p++) {
      var period = periods[p];
      var interval = period.getChild('timeInterval');
      if (!interval) {
        ctx.log('WAARSCHUWING: Period zonder timeInterval', { ts: j + 1, period: p + 1 });
        continue;
      }

      var resolution = period.getChildText('resolution');
      var step = resolutionSeconds_(resolution);
      if (step === null) {
        ctx.log('WAARSCHUWING: onbekende resolutie, Period overgeslagen',
          { ts: j + 1, resolution: resolution });
        continue;
      }

      var startEpoch = new Date(interval.getChildText('start')).getTime();
      if (isNaN(startEpoch)) {
        ctx.log('WAARSCHUWING: onleesbare starttijd',
          { ts: j + 1, start: interval.getChildText('start') });
        continue;
      }
      var endEpoch = new Date(interval.getChildText('end')).getTime();

      jobs.push({
        ts: j + 1,
        period: p + 1,
        curveType: curveType,
        resolution: resolution,
        step: step,
        start: startEpoch / 1000,
        end: isNaN(endEpoch) ? null : endEpoch / 1000,
        points: period.getChildren('Point')
      });
    }
  }

  jobs.sort(function (a, b) { return b.step - a.step; }); // grof eerst

  var byTime = {};
  var resoluties = {};
  var curveTypes = {};
  var gevuld = 0;

  for (var k = 0; k < jobs.length; k++) {
    var job = jobs[k];
    resoluties[job.resolution] = (resoluties[job.resolution] || 0) + 1;
    curveTypes[job.curveType] = (curveTypes[job.curveType] || 0) + 1;

    var priceAt = {};
    var maxPos = 0;
    for (var i = 0; i < job.points.length; i++) {
      var rawPos = job.points[i].getChildText('position');
      var rawPrice = job.points[i].getChildText('price.amount');
      var position = parseInt(rawPos, 10);
      var price = parseFloat(rawPrice);

      if (isNaN(position) || isNaN(price)) {
        ctx.log('WAARSCHUWING: punt overgeslagen',
          { ts: job.ts, period: job.period, position: rawPos, prijs: rawPrice });
        continue;
      }
      priceAt[position] = price;
      if (position > maxPos) maxPos = position;
    }

    if (maxPos === 0) continue;

    var expected = maxPos;
    if (job.end !== null && job.end > job.start) {
      var n = Math.round((job.end - job.start) / job.step);
      if (n > 0 && n < 20000) expected = n;
    }
    if (expected < maxPos) expected = maxPos;

    // Bij A03/A04/A05 geldt een prijs tot de volgende expliciete positie, dus
    // ontbrekende posities forward-fillen. Bij A01 horen er geen gaten te zijn.
    var variabel = (job.curveType === 'A03' || job.curveType === 'A04' || job.curveType === 'A05');
    var limit = variabel ? expected : maxPos;

    var last = null;
    for (var pos = 1; pos <= limit; pos++) {
      if (priceAt.hasOwnProperty(pos)) {
        last = priceAt[pos];
      } else {
        if (last === null) continue;
        gevuld++;
        if (!variabel) {
          ctx.log('WAARSCHUWING: gat in een ' + job.curveType + ' reeks, forward-filled',
            { ts: job.ts, position: pos });
        }
      }
      var t = job.start + (pos - 1) * job.step;
      byTime[t] = { price: last * factor, step: job.step };
    }
  }

  var times = Object.keys(byTime).sort(function (a, b) { return a - b; });
  var jsonData = [];
  for (var m = 0; m < times.length; m++) {
    jsonData.push({ time: Number(times[m]), price: byTime[times[m]].price });
  }

  ctx.log('punten verzameld', {
    aantal: jsonData.length,
    forwardFilled: gevuld,
    resoluties: resoluties,
    curveTypes: curveTypes,
    factor: factor,
    eerste: jsonData.length ? new Date(jsonData[0].time * 1000).toISOString() : null,
    laatste: jsonData.length ? new Date(jsonData[jsonData.length - 1].time * 1000).toISOString() : null
  });

  if (jsonData.length === 0) {
    throw new Error('TimeSeries aanwezig maar geen bruikbare punten voor ' + ctx.country);
  }

  return jsonData;
}

/* ------------------------------------------------------------------ */
/* Context, logging en response                                        */
/* ------------------------------------------------------------------ */

function newContext_(e) {
  var t0 = Date.now();
  var params = (e && e.parameter) || {};

  var ctx = {
    id: Utilities.getUuid().substring(0, 8),
    t0: t0,
    e: e || {},
    country: params.country || DEFAULT_COUNTRY,
    domain: null,
    currency: null,
    xmlUrl: null,
    step: 'init',
    trace: [],
    fromCache: false,
    debug: params.debug === '1',
    noCache: params.nocache === '1',
    bron: (params.bron === 'nordpool' || params.bron === 'entsoe') ? params.bron : null,
    source: null
  };

  ctx.elapsed = function () { return Date.now() - t0; };

  ctx.log = function (msg, data) {
    var entry = { ms: Date.now() - t0, stap: ctx.step, bericht: msg };
    if (data !== undefined) entry.data = data;
    ctx.trace.push(entry);
    console.log('[' + ctx.id + '] ' + ctx.step + ' | ' + msg +
      (data === undefined ? '' : ' ' + safeJson_(data)));
  };

  ctx.fail = function (err) {
    ctx.trace.push({ ms: Date.now() - t0, stap: ctx.step, fout: String((err && err.message) || err) });
    console.error('[' + ctx.id + '] MISLUKT in stap "' + ctx.step + '" na ' + ctx.elapsed() + ' ms\n' +
      (err && err.stack ? err.stack : String(err)) + '\n' +
      'context: ' + safeJson_({
        country: ctx.country,
        domain: ctx.domain,
        periodStart: ctx.periodStart,
        periodEnd: ctx.periodEnd,
        fetchMs: ctx.fetchMs,
        bron: ctx.source,
        parameter: ctx.e.parameter
      }));
  };

  return ctx;
}

function reply_(ctx, body, jsonData) {
  if (ctx.debug) {
    return ContentService
      .createTextOutput(JSON.stringify({
        reqId: ctx.id,
        country: ctx.country,
        domain: ctx.domain,
        uitCache: ctx.fromCache,
        bron: ctx.source,
        ms: ctx.elapsed(),
        punten: jsonData.length,
        trace: ctx.trace,
        data: jsonData
      }, null, 2))
      .setMimeType(ContentService.MimeType.JSON);
  }
  return ContentService.createTextOutput(body).setMimeType(ContentService.MimeType.JSON);
}

function replyError_(ctx, err) {
  // Apps Script kan geen HTTP-statuscode zetten: de fout gaat als JSON mee met een 200.
  var payload = {
    error: true,
    reqId: ctx.id,
    stap: ctx.step,
    bericht: String((err && err.message) || err),
    country: ctx.country,
    domain: ctx.domain,
    ms: ctx.elapsed()
  };
  if (ctx.debug) {
    payload.trace = ctx.trace;
    payload.stack = err && err.stack ? String(err.stack) : null;
  }
  return ContentService.createTextOutput(JSON.stringify(payload, null, 2))
    .setMimeType(ContentService.MimeType.JSON);
}

function safeJson_(v) {
  try {
    var s = JSON.stringify(v);
    return s.length > 2000 ? s.substring(0, 2000) + '...[afgekapt]' : s;
  } catch (err) {
    return '[niet serialiseerbaar: ' + err.message + ']';
  }
}

/* ------------------------------------------------------------------ */
/* Cache met vangnet                                                   */
/* ------------------------------------------------------------------ */

function xmlCacheKey_(domain, periodStart, periodEnd) {
  return 'xml_' + domain + '_' + periodStart + '_' + periodEnd;
}

function safeGet_(ctx, key) {
  try {
    return CacheService.getScriptCache().get(key);
  } catch (err) {
    ctx.log('cache.get mislukt', { key: key, fout: String(err.message || err) });
    return null;
  }
}

function safePut_(ctx, key, value, ttl) {
  if (typeof value !== 'string') value = String(value);

  if (value.length > CACHE_SAFE_BYTES) {
    // CacheService weigert waarden boven 100 KB.
    ctx.log('cache.put OVERGESLAGEN, waarde te groot', { key: key, bytes: value.length });
    return false;
  }
  if (key.length > 250) {
    ctx.log('cache.put OVERGESLAGEN, sleutel te lang', { bytes: key.length });
    return false;
  }

  try {
    CacheService.getScriptCache().put(key, value, ttl);
    return true;
  } catch (err) {
    ctx.log('cache.put mislukt', { key: key, bytes: value.length, fout: String(err.message || err) });
    return false;
  }
}

/* ------------------------------------------------------------------ */
/* Hulpfuncties                                                        */
/* ------------------------------------------------------------------ */

function getDescendantsByTagName(element, tagName) {
  var matchingElements = [];

  function findDescendants(currentElement) {
    var children = currentElement.getChildren();
    for (var i = 0; i < children.length; i++) {
      var child = children[i];
      if (child.getName() === tagName) matchingElements.push(child);
      findDescendants(child);
    }
  }

  findDescendants(element);
  return matchingElements;
}

function getFormattedMidnightUTC(offsetDays) {
  var now = new Date();
  var localMidnight = new Date(now.getFullYear(), now.getMonth(), now.getDate() + offsetDays, 0, 0, 0, 0);

  var utcMidnight = new Date(localMidnight.getTime());
  var year = utcMidnight.getUTCFullYear();
  var month = ('0' + (utcMidnight.getUTCMonth() + 1)).slice(-2);
  var day = ('0' + utcMidnight.getUTCDate()).slice(-2);
  var hours = ('0' + utcMidnight.getUTCHours()).slice(-2);
  var minutes = ('0' + utcMidnight.getUTCMinutes()).slice(-2);

  return year + month + day + hours + minutes;
}

/* ------------------------------------------------------------------ */
/* Handmatige testhulpjes, uit te voeren vanuit de editor              */
/* ------------------------------------------------------------------ */

function test_NL() { return test_('NL'); }
function test_NO2() { return test_('NO2'); }

function test_(country) {
  var out = doGet({ parameter: { country: country, debug: '1', nocache: '1' } });
  var txt = out.getContent();
  console.log(txt.substring(0, 4000));
  return txt;
}

/**
 * Alle relevante cache-entries wissen, inclusief de xml_ sleutels van de
 * afgelopen dagen. Elke aanroepdag k gebruikt het venster (k-1, k+2).
 */
function clearCaches() {
  var keys = [];
  var landen = Object.keys(ZONES);
  for (var i = 0; i < landen.length; i++) {
    keys.push('output' + landen[i]);
    keys.push('lastgood_' + landen[i]);
    keys.push('poging_' + landen[i]);
    keys.push('gatpoging_' + landen[i]);
    for (var k = -3; k <= 0; k++) {
      keys.push(xmlCacheKey_(ZONES[landen[i]].domain,
        getFormattedMidnightUTC(k - 1), getFormattedMidnightUTC(k + 2)));
    }
  }
  keys.push('currencyNOK', 'currencyDKK', 'currencySEK');

  for (var s = 0; s < keys.length; s += 100) {
    CacheService.getScriptCache().removeAll(keys.slice(s, s + 100));
  }
  console.log('cache gewist: ' + keys.length + ' sleutels');
}
