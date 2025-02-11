function doGet(e) {

  var logger = Logger.log;
  var country = e?.parameter?.country || 'NL';
  var cache = CacheService.getScriptCache();

  ExchangeApp.setRatesEndpoint("https://openexchangerates.org/api/latest.json?app_id=*************", 3600)

  var cachedData = cache.get("output" + country);
  if (cachedData != null) {
    logger('from cache');
    logger('All Data: ' + JSON.stringify(cachedData));
    return ContentService.createTextOutput(cachedData)
      .setMimeType(ContentService.MimeType.JSON);
  }

  var factor = 1;
  if (country.startsWith('NO')) {
    // https://github.com/bkper/exchange-app
    factor = cache.get("currencyNOK");
    if (factor == null) {
      factor = ExchangeApp.convert(1, 'EUR', 'NOK');
      // factor = 11.79;
      cache.put("currencyNOK", factor, 3600 * 24);
    }
  }

  if (country.startsWith('DK')) {
    // https://github.com/bkper/exchange-app
    factor = cache.get("currencyDKK");
    if (factor == null) {
      factor = ExchangeApp.convert(1, 'EUR', 'DKK');
      // factor = 7.46;
      cache.put("currencyDKK", factor, 3600 * 24);
    }
  }

  if (country.startsWith('SE')) {
    // https://github.com/bkper/exchange-app
    factor = cache.get("currencySEK");
    if (factor == null) {
      factor = ExchangeApp.convert(1, 'EUR', 'SEK');
      // factor = 11.59;
      cache.put("currencySEK", factor, 3600 * 24);
    }
  }

  var domain = lookupValue(country);
  logger(country + ': ' + domain + ' currency: ' + factor);

  var xmlUrl = 'https://web-api.tp.entsoe.eu/api?documentType=A44&out_Domain=' + domain + '&in_Domain=' + domain + '&periodStart=' + getFormattedMidnightUTC(-1) + '&periodEnd=' + getFormattedMidnightUTC(2) + '&securityToken=57ba8658-37a5-4e23-9bee-e60fc3f954db';
  logger(xmlUrl);
  var cachedXml = cache.get(xmlUrl);

  if (true && cachedXml) {
    logger('Using cached XML content.');
    var xmlContent = cachedXml;
  } else {
    logger('Fetching XML content from ' + xmlUrl);
    var response = UrlFetchApp.fetch(xmlUrl);
    var xmlContent = response.getContentText();
    cache.put(xmlUrl, xmlContent, 3600);
  }

  xmlContent = xmlContent.replace(/xmlns(:[a-z0-9]+)?=["'][^"']*["']/g, '');
  xmlContent = xmlContent.replace(/<([a-z0-9]+):/g, '<').replace(/<\/([a-z0-9]+):/g, '</');

  var document = XmlService.parse(xmlContent);
  var root = document.getRootElement();

  var timeSeriesList = getDescendantsByTagName(root, 'TimeSeries');

  if (timeSeriesList.length > 0) {
    var jsonData = [];


    for (var j = 0; j < timeSeriesList.length; j++) {
      var timeSeries = timeSeriesList[j];

      var period = timeSeries.getChild('Period');

      if (period) {
        var startTime = period.getChild('timeInterval').getChildText('start');
        logger(startTime);
        var points = period.getChildren('Point');

        for (var i = 0; i < points.length; i++) {
          var position = points[i].getChildText('position');
          var priceAmount = points[i].getChildText('price.amount') * factor;

          // Convert ISO date to epoch time (in seconds)
          var epochTime = new Date(startTime).getTime() / 1000 + (position - 1) * 3600;

          jsonData.push({
            time: epochTime,
            price: parseFloat(priceAmount)
          });
        }

      } else {
        logger('Error: No <Period> element found in TimeSeries ' + (j + 1));
      }
    }

    if (jsonData.length > 36) {
      jsonData.splice(0, jsonData.length - 36);
    }

    cache.put("output" + country, JSON.stringify(jsonData), 3600);
    logger('All Data: ' + JSON.stringify(jsonData));

    return ContentService.createTextOutput(JSON.stringify(jsonData))
      .setMimeType(ContentService.MimeType.JSON);
  } else {
    logger('Error: No <TimeSeries> elements found in Publication_MarketDocument');
    return ContentService.createTextOutput('Error: No <TimeSeries> elements found in Publication_MarketDocument');
  }
}

function getDescendantsByTagName(element, tagName) {
  var matchingElements = [];

  function findDescendants(currentElement) {
    var children = currentElement.getChildren();

    for (var i = 0; i < children.length; i++) {
      var child = children[i];
      if (child.getName() === tagName) {
        matchingElements.push(child);
      }
      findDescendants(child);
    }
  }

  findDescendants(element);
  return matchingElements;
}

function getFormattedMidnightUTC(offsetDays) {
  var now = new Date(); // current date and time
  var localMidnight = new Date(now.getFullYear(), now.getMonth(), now.getDate() + offsetDays, 0, 0, 0, 0);

  var utcMidnight = new Date(localMidnight.getTime());
  var year = utcMidnight.getUTCFullYear();
  var month = ('0' + (utcMidnight.getUTCMonth() + 1)).slice(-2); // Months are zero-based
  var day = ('0' + utcMidnight.getUTCDate()).slice(-2);
  var hours = ('0' + utcMidnight.getUTCHours()).slice(-2);
  var minutes = ('0' + utcMidnight.getUTCMinutes()).slice(-2);

  var formattedTime = year + month + day + hours + minutes;
  return formattedTime;
}

// https://transparency.entsoe.eu/content/static_content/Static%20content/web%20api/Guide.html#_areas
// https://transparencyplatform.zendesk.com/hc/en-us/articles/15885757676308-Area-List-with-Energy-Identification-Code-EIC

function lookupValue(country) {
  var countryValues = {
    'AT': '10YAT-APG------L',
    'BE': '10YBE----------2',
    'CH': '10YCH-SWISSGRIDZ',
    'CZ': '10YCZ-CEPS-----N',
    'DE': '10Y1001A1001A82H',
    'DK1': '10YDK-1--------W',
    'DK2': '10YDK-2--------M',
    'EE': '10Y1001A1001A39I',
    'ES': '10YES-REE------0',
    'FI': '10YFI-1--------U',
    'FR': '10YFR-RTE------C',
    'LT': '10YLT-1001A0008Q',
    'LU': '10Y1001A1001A82H',
    'LV': '10YLV-1001A00074',
    'NL': '10YNL----------L',
    'NO1': '10YNO-1--------2',
    'NO2': '10YNO-2--------T',
    'NO3': '10YNO-3--------J',
    'NO4': '10YNO-4--------9',
    'NO5': '10Y1001A1001A48H',
    'PL': '10YPL-AREA-----S',
    'PT': '10YPT-REN------W',
    'RO': '10YRO-TEL------P',
    'SE1': '10Y1001A1001A44P',
    'SE2': '10Y1001A1001A45N',
    'SE3': '10Y1001A1001A46L',
    'SE4': '10Y1001A1001A47J',
    'SI': '10YSI-ELES-----O',
    'SK': '10YSK-SEPS-----K'
  };
  var value = countryValues[country] || '10YNL----------L';
  return value;
}
