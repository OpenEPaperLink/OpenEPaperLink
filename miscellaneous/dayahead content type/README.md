# Day Ahead Energy Prices

Note: this script is for reference and documentation only. You don't need to deploy it yourself to use the Day Ahead content in OpenEpaperLink, as it makes use of a common install.

`get_dayahead.js` is the Google Apps Script behind that common install. It returns `[{"time": <epoch>, "price": <per MWh>}]` for yesterday, today and tomorrow.

Sources:

- Nord Pool is the primary source for the zones it covers (AT, BE, DE/LU, DK1-2, EE, FI, FR, LT, LV, NL, NO1-5, PL, SE1-4), in the zone's own currency.
- ENTSO-E is the fallback, and the only source for CH, CZ, ES, PT, RO, SI and SK. ENTSO-E sometimes misses a whole day (NL on 10 October 2026), so a cached set with a gap in today is fetched again.

To deploy your own copy, set these script properties (Project settings, Script properties):

- `ENTSOE_TOKEN`: your ENTSO-E Transparency Platform security token
- `OXR_APP_ID`: an openexchangerates.org app id, only needed for the NOK, DKK and SEK zones when they fall back to ENTSO-E

Call it with `?country=NL` (see `ZONES` in the script for the codes). `&debug=1` returns a trace of what the script did, `&nocache=1` skips the caches, and `&bron=nordpool` or `&bron=entsoe` forces a source.
