# Day Ahead Energy Prices

Note: this script is for reference and documentation only. You don't need to deploy it yourself to use the Day Ahead content in OpenEpaperLink, as it makes use of a common install.

`get_dayahead.js` is the Google Apps Script behind that common install. To deploy your own copy, set these script properties (Project settings, Script properties):

- `ENTSOE_TOKEN`: your ENTSO-E Transparency Platform security token
- `OXR_APP_ID`: an openexchangerates.org app id, only needed for the NOK, DKK and SEK zones

Call it with `?country=NL` (see `ZONES` in the script for the codes). `&debug=1` returns a trace of what the script did, `&nocache=1` skips the caches.
