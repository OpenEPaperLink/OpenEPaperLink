#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "tag_db.h"
#include "makeimage.h"
#include "TFT_eSPI.h"
#include "contentmanager.h"
#include "web.h"
#include <DrawOWM.h>

#define ENABLE_LOGGING  1
#if ENABLE_LOGGING && __has_include("logging.h") 
#include "logging.h"
#else
#define LOG(format, ...)
#define LOG_RAW(format, ...)
#endif

bool HttpQuery(String &url,String &Response);

bool OwmWeather(TFT_eSprite &spr, JsonObject &cfgobj, const tagRecord *taginfo, imgParam &imageParams)
{
   bool Ret = false; // Assume the worse

// Don't add timestamp to our display, we draw our own
   imageParams.ts_option = 0; 
   do {
      OwmConfig Config;
      LOG("Owm test\n");

      if(imageParams.width >=  800 && imageParams.height >= 480) {
         Config.DisplayFormat = FORMAT_800X480;
      }
      else if(imageParams.width >= 640 && imageParams.height >= 384) {
         Config.DisplayFormat = FORMAT_640X384;
      }
      else if(imageParams.width >= 400 | imageParams.height >= 300) {
         Config.DisplayFormat = FORMAT_400X300;
      }
      else {
         LOG("%d x %d display not supported\n",imageParams.width,imageParams.height);
         break;
      }

      getLocation(cfgobj);
      String City = cfgobj["location"];
      Config.City = City.c_str();
      Config.bMetric = cfgobj["units"] == "0";

      Config.WindSpeed = Config.bMetric ? UNITS_SPEED_KILOMETERSPERHOUR : 
                     UNITS_SPEED_MILESPERHOUR;
      Config.DistanceType = Config.bMetric ? UNITS_DIST_KILOMETERS : UNITS_DIST_MILES;
      Config.PrecipType = Config.bMetric ? UNITS_DAILY_PRECIP_MILLIMETERS : 
                                       UNITS_DAILY_PRECIP_INCHES;

      Config.PrecipHrType = Config.bMetric ? UNITS_HOURLY_PRECIP_MILLIMETERS :
                                         UNITS_HOURLY_PRECIP_INCHES;
      Config.PressureType = Config.bMetric ? UNITS_PRES_MILLIBARS :
                                         UNITS_PRES_INCHESOFMERCURY;

      Config.bDisplayAlerts = Config.bMetric ? false : true;

      Config.inTemp         = taginfo->temperature;
      Config.Rssi           = taginfo->RSSI;
      Config.batteryVoltage = taginfo->batteryMv;

      Config.bLiPo = false;
      Config.TimeFormat = "%l:%M %P";
      Config.DateFormat = "%a, %B %e";
      Config.inHumidity = NAN;

      Config.PosSunrise      = 0;
      Config.PosSunset       = 1;
      Config.PosWind         = 2;
      Config.PosHumidity     = 3;
      Config.PosUvi          = 4;
      Config.PosPressure     = 5;
      Config.PosAirQuality   = 6;
      Config.PosVisibility   = 7;
      Config.PosIntemp       = 8;
      Config.PosDewpoint     = 9;
      Config.PosInhumidity   = -1;
      Config.PosMoonrise     = -1;
      Config.PosMoonset      = -1;
      Config.PosMoonphase    = -1;

      switch(Config.DisplayFormat) {
         case FORMAT_800X480:
            Config.DisplayWidth    = 800;
            Config.DisplayHeight   = 480;
            break;

         case FORMAT_640X384:
     // positions 6,7,8,9 are not available on the 640 x 384 display
            Config.DisplayWidth    = 640;
            Config.DisplayHeight   = 384;
            Config.PosVisibility   = 4;
            Config.PosIntemp       = 5;
            Config.PosUvi          = -1;
            Config.PosPressure     = -1;
            Config.PosAirQuality   = -1;
            Config.PosInhumidity   = -1;
            break;

         case FORMAT_400X300:
            Config.DisplayWidth    = 400;
            Config.DisplayHeight   = 300;
            break;
      }

      Config.xOffset = (imageParams.width - Config.DisplayWidth) / 2;
      Config.yOffset = (imageParams.height - Config.DisplayHeight) / 2;

      String url = "http://api.openweathermap.org/data/3.0/onecall?lat=";
      url += cfgobj["#lat"].as<String>() + "&lon=" + cfgobj["#lon"].as<String>() 
             + "&lang=" + "en" /* fix me */
             + "&units=standard&exclude=minutelyalerts&appid="
             + config.owmApiKey;
      String ForecastResponse;
      if(!HttpQuery(url,ForecastResponse)) {
         ELOG("\n");
         break;
      }
      Config.ForecastApiResponse = ForecastResponse.c_str();
      String OwmPollutionResponse;

      if(Config.PosAirQuality != -1) {
      // set start and end to appropriate values so that the last 24 hours 
      // of air pollution history is returned. Unix, UTC.
         time_t now;
         int64_t end = time(&now);
      // minus 1 is important here, otherwise we could get an extra hour of history
         int64_t start = end - ((3600 * 24) - 1);
         char endStr[22];
         char startStr[22];
         sprintf(endStr, "%lld", end);
         sprintf(startStr, "%lld", start);

         url = "http://api.openweathermap.org/data/2.5/air_pollution/history?lat=";
         url += cfgobj["#lat"].as<String>() += "&lon=" 
                + cfgobj["#lon"].as<String>() + "&start=" + startStr 
                + "&end=" + endStr + "&appid=" + config.owmApiKey;

         if(!HttpQuery(url,OwmPollutionResponse)) {
            ELOG("\n");
            break;
         }
         Config.AirPollutionApiResponse = OwmPollutionResponse.c_str();
         // LOG("response = %s\n",Config.AirPollutionApiResponse);
      }
      else {
         Config.AirPollutionApiResponse = NULL;
      }

      class DrawOWM *owm = new DrawOWM(spr,Config);
      owm->DrawIt();
      delete owm;
      Ret = true;
   } while(false);

   return Ret;
}

bool HttpQuery(String &url,String &Response)
{
   HTTPClient http;
   bool Ret = false;

   // LOG("Fetching %s\n",url.c_str());

   http.begin(url);
   http.setTimeout(5000);
   http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
   int httpCode = http.GET();
   if (httpCode != 200) {
       http.end();
       String Err = "[HttpQuery] ";
       Err += url + " code " + httpCode;
       wsErr(Err);
       LOG("http.GET() failed: %s\n",Err.c_str());
       String Err1 = http.getString();
       wsErr(Err1);
       LOG("%s\n",Err1.c_str());
   }
   else {
      Response = http.getString().c_str();
      Ret = true;
   }
   http.end();

   return Ret;
}
