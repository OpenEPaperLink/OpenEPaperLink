#include <Arduino.h>

#include "TFT_eSPI.h"
#include <DrawOWM.h>

#define ENABLE_LOGGING  1
#if ENABLE_LOGGING && __has_include("logging.h") 
#include "logging.h"
#else
#define LOG(format, ...)
#define LOG_RAW(format, ...)
#endif

const char *OwmForecastTestResponse = 
"{\"lat\":33.7641,\
\"lon\":-118.3391,\
\"timezone\":\"America/Los_Angeles\",\
\"timezone_offset\":-25200,\
\"current\":{\"dt\":1779805577,\
\"sunrise\":1779799567,\
\"sunset\":1779850503,\
\"temp\":287.62,\
\"feels_like\":287.26,\
\"pressure\":1011,\
\"humidity\":82,\
\"dew_point\":284.59,\
\"uvi\":0.12,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":3.6,\
\"wind_deg\":220,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}]},\
\"hourly\":[{\"dt\":1779804000,\
\"temp\":287.62,\
\"feels_like\":287.26,\
\"pressure\":1011,\
\"humidity\":82,\
\"dew_point\":284.59,\
\"uvi\":0.12,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":3.67,\
\"wind_deg\":187,\
\"wind_gust\":3.87,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779807600,\
\"temp\":287.68,\
\"feels_like\":287.3,\
\"pressure\":1011,\
\"humidity\":81,\
\"dew_point\":284.46,\
\"uvi\":0.35,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":3.94,\
\"wind_deg\":173,\
\"wind_gust\":4.22,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779811200,\
\"temp\":288.02,\
\"feels_like\":287.6,\
\"pressure\":1011,\
\"humidity\":78,\
\"dew_point\":284.22,\
\"uvi\":1.03,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":4.47,\
\"wind_deg\":168,\
\"wind_gust\":4.59,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779814800,\
\"temp\":288.39,\
\"feels_like\":287.95,\
\"pressure\":1011,\
\"humidity\":76,\
\"dew_point\":284.19,\
\"uvi\":2.09,\
\"clouds\":99,\
\"visibility\":10000,\
\"wind_speed\":3.61,\
\"wind_deg\":169,\
\"wind_gust\":3.54,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779818400,\
\"temp\":288.96,\
\"feels_like\":288.55,\
\"pressure\":1012,\
\"humidity\":75,\
\"dew_point\":284.54,\
\"uvi\":3.7,\
\"clouds\":99,\
\"visibility\":10000,\
\"wind_speed\":2.52,\
\"wind_deg\":168,\
\"wind_gust\":2.71,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779822000,\
\"temp\":290,\
\"feels_like\":289.67,\
\"pressure\":1011,\
\"humidity\":74,\
\"dew_point\":283.61,\
\"uvi\":5.24,\
\"clouds\":99,\
\"visibility\":10000,\
\"wind_speed\":2.11,\
\"wind_deg\":198,\
\"wind_gust\":2.51,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779825600,\
\"temp\":290.97,\
\"feels_like\":290.69,\
\"pressure\":1011,\
\"humidity\":72,\
\"dew_point\":283.78,\
\"uvi\":7.89,\
\"clouds\":99,\
\"visibility\":10000,\
\"wind_speed\":2.98,\
\"wind_deg\":242,\
\"wind_gust\":2.91,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779829200,\
\"temp\":291.26,\
\"feels_like\":290.95,\
\"pressure\":1010,\
\"humidity\":70,\
\"dew_point\":283.75,\
\"uvi\":7.28,\
\"clouds\":97,\
\"visibility\":10000,\
\"wind_speed\":3.74,\
\"wind_deg\":245,\
\"wind_gust\":3.37,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779832800,\
\"temp\":291.42,\
\"feels_like\":291.1,\
\"pressure\":1009,\
\"humidity\":69,\
\"dew_point\":283.85,\
\"uvi\":6.2,\
\"clouds\":96,\
\"visibility\":10000,\
\"wind_speed\":4.77,\
\"wind_deg\":253,\
\"wind_gust\":4.41,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779836400,\
\"temp\":291.24,\
\"feels_like\":290.88,\
\"pressure\":1009,\
\"humidity\":68,\
\"dew_point\":283.8,\
\"uvi\":4.18,\
\"clouds\":85,\
\"visibility\":10000,\
\"wind_speed\":6.36,\
\"wind_deg\":262,\
\"wind_gust\":6.13,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779840000,\
\"temp\":290.57,\
\"feels_like\":290.2,\
\"pressure\":1008,\
\"humidity\":70,\
\"dew_point\":284.21,\
\"uvi\":2.24,\
\"clouds\":74,\
\"visibility\":10000,\
\"wind_speed\":7.22,\
\"wind_deg\":264,\
\"wind_gust\":7.59,\
\"weather\":[{\"id\":803,\
\"main\":\"Clouds\",\
\"description\":\"broken clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779843600,\
\"temp\":289.63,\
\"feels_like\":289.19,\
\"pressure\":1008,\
\"humidity\":71,\
\"dew_point\":284.36,\
\"uvi\":0.91,\
\"clouds\":7,\
\"visibility\":10000,\
\"wind_speed\":8.25,\
\"wind_deg\":263,\
\"wind_gust\":8.04,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01d\"}],\
\"pop\":0},\
{\"dt\":1779847200,\
\"temp\":288.56,\
\"feels_like\":287.85,\
\"pressure\":1008,\
\"humidity\":65,\
\"dew_point\":282.77,\
\"uvi\":0.23,\
\"clouds\":6,\
\"visibility\":10000,\
\"wind_speed\":7.86,\
\"wind_deg\":269,\
\"wind_gust\":7.69,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01d\"}],\
\"pop\":0},\
{\"dt\":1779850800,\
\"temp\":287.35,\
\"feels_like\":286.52,\
\"pressure\":1008,\
\"humidity\":65,\
\"dew_point\":282.17,\
\"uvi\":0,\
\"clouds\":3,\
\"visibility\":10000,\
\"wind_speed\":7.77,\
\"wind_deg\":273,\
\"wind_gust\":7.7,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01n\"}],\
\"pop\":0},\
{\"dt\":1779854400,\
\"temp\":286.7,\
\"feels_like\":285.86,\
\"pressure\":1008,\
\"humidity\":67,\
\"dew_point\":282.45,\
\"uvi\":0,\
\"clouds\":2,\
\"visibility\":10000,\
\"wind_speed\":7.59,\
\"wind_deg\":270,\
\"wind_gust\":7.54,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01n\"}],\
\"pop\":0},\
{\"dt\":1779858000,\
\"temp\":286.18,\
\"feels_like\":285.26,\
\"pressure\":1008,\
\"humidity\":66,\
\"dew_point\":282.17,\
\"uvi\":0,\
\"clouds\":2,\
\"visibility\":10000,\
\"wind_speed\":7.16,\
\"wind_deg\":268,\
\"wind_gust\":6.99,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01n\"}],\
\"pop\":0},\
{\"dt\":1779861600,\
\"temp\":285.97,\
\"feels_like\":285.08,\
\"pressure\":1008,\
\"humidity\":68,\
\"dew_point\":282.12,\
\"uvi\":0,\
\"clouds\":3,\
\"visibility\":10000,\
\"wind_speed\":6.37,\
\"wind_deg\":264,\
\"wind_gust\":6.15,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01n\"}],\
\"pop\":0},\
{\"dt\":1779865200,\
\"temp\":286.36,\
\"feels_like\":285.54,\
\"pressure\":1008,\
\"humidity\":69,\
\"dew_point\":282.12,\
\"uvi\":0,\
\"clouds\":25,\
\"visibility\":10000,\
\"wind_speed\":5.29,\
\"wind_deg\":254,\
\"wind_gust\":5.49,\
\"weather\":[{\"id\":802,\
\"main\":\"Clouds\",\
\"description\":\"scattered clouds\",\
\"icon\":\"03n\"}],\
\"pop\":0},\
{\"dt\":1779868800,\
\"temp\":286.33,\
\"feels_like\":285.51,\
\"pressure\":1008,\
\"humidity\":69,\
\"dew_point\":282.1,\
\"uvi\":0,\
\"clouds\":57,\
\"visibility\":10000,\
\"wind_speed\":4.76,\
\"wind_deg\":242,\
\"wind_gust\":5.22,\
\"weather\":[{\"id\":803,\
\"main\":\"Clouds\",\
\"description\":\"broken clouds\",\
\"icon\":\"04n\"}],\
\"pop\":0},\
{\"dt\":1779872400,\
\"temp\":286.47,\
\"feels_like\":285.63,\
\"pressure\":1008,\
\"humidity\":68,\
\"dew_point\":281.93,\
\"uvi\":0,\
\"clouds\":72,\
\"visibility\":10000,\
\"wind_speed\":4.49,\
\"wind_deg\":240,\
\"wind_gust\":4.99,\
\"weather\":[{\"id\":803,\
\"main\":\"Clouds\",\
\"description\":\"broken clouds\",\
\"icon\":\"04n\"}],\
\"pop\":0},\
{\"dt\":1779876000,\
\"temp\":286.39,\
\"feels_like\":285.55,\
\"pressure\":1008,\
\"humidity\":68,\
\"dew_point\":281.88,\
\"uvi\":0,\
\"clouds\":80,\
\"visibility\":10000,\
\"wind_speed\":3.29,\
\"wind_deg\":232,\
\"wind_gust\":3.74,\
\"weather\":[{\"id\":803,\
\"main\":\"Clouds\",\
\"description\":\"broken clouds\",\
\"icon\":\"04n\"}],\
\"pop\":0},\
{\"dt\":1779879600,\
\"temp\":286.45,\
\"feels_like\":285.61,\
\"pressure\":1008,\
\"humidity\":68,\
\"dew_point\":281.92,\
\"uvi\":0,\
\"clouds\":85,\
\"visibility\":10000,\
\"wind_speed\":3.23,\
\"wind_deg\":222,\
\"wind_gust\":3.59,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04n\"}],\
\"pop\":0},\
{\"dt\":1779883200,\
\"temp\":286.36,\
\"feels_like\":285.54,\
\"pressure\":1008,\
\"humidity\":69,\
\"dew_point\":282.06,\
\"uvi\":0,\
\"clouds\":88,\
\"visibility\":10000,\
\"wind_speed\":3.35,\
\"wind_deg\":222,\
\"wind_gust\":3.67,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04n\"}],\
\"pop\":0},\
{\"dt\":1779886800,\
\"temp\":286.22,\
\"feels_like\":285.38,\
\"pressure\":1009,\
\"humidity\":69,\
\"dew_point\":282.1,\
\"uvi\":0,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":2.79,\
\"wind_deg\":224,\
\"wind_gust\":3.08,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779890400,\
\"temp\":286.32,\
\"feels_like\":285.47,\
\"pressure\":1009,\
\"humidity\":68,\
\"dew_point\":281.83,\
\"uvi\":0.14,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":2.58,\
\"wind_deg\":220,\
\"wind_gust\":2.96,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779894000,\
\"temp\":286.33,\
\"feels_like\":285.48,\
\"pressure\":1009,\
\"humidity\":68,\
\"dew_point\":281.75,\
\"uvi\":0.56,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":2.46,\
\"wind_deg\":211,\
\"wind_gust\":2.8,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779897600,\
\"temp\":286.52,\
\"feels_like\":285.69,\
\"pressure\":1010,\
\"humidity\":68,\
\"dew_point\":281.85,\
\"uvi\":1.68,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":2.98,\
\"wind_deg\":211,\
\"wind_gust\":3.33,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779901200,\
\"temp\":286.66,\
\"feels_like\":285.84,\
\"pressure\":1010,\
\"humidity\":68,\
\"dew_point\":282.08,\
\"uvi\":3.08,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":4.06,\
\"wind_deg\":228,\
\"wind_gust\":4.36,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779904800,\
\"temp\":287.05,\
\"feels_like\":286.22,\
\"pressure\":1011,\
\"humidity\":66,\
\"dew_point\":281.94,\
\"uvi\":3.35,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":4.95,\
\"wind_deg\":247,\
\"wind_gust\":5.25,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779908400,\
\"temp\":287.17,\
\"feels_like\":286.32,\
\"pressure\":1011,\
\"humidity\":65,\
\"dew_point\":281.83,\
\"uvi\":4.53,\
\"clouds\":100,\
\"visibility\":10000,\
\"wind_speed\":5.98,\
\"wind_deg\":266,\
\"wind_gust\":6.35,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779912000,\
\"temp\":287.43,\
\"feels_like\":286.53,\
\"pressure\":1011,\
\"humidity\":62,\
\"dew_point\":281.45,\
\"uvi\":5.46,\
\"clouds\":90,\
\"visibility\":10000,\
\"wind_speed\":5.03,\
\"wind_deg\":261,\
\"wind_gust\":5.58,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779915600,\
\"temp\":288.12,\
\"feels_like\":287.21,\
\"pressure\":1011,\
\"humidity\":59,\
\"dew_point\":281.32,\
\"uvi\":4.64,\
\"clouds\":57,\
\"visibility\":10000,\
\"wind_speed\":6.03,\
\"wind_deg\":247,\
\"wind_gust\":6.66,\
\"weather\":[{\"id\":803,\
\"main\":\"Clouds\",\
\"description\":\"broken clouds\",\
\"icon\":\"04d\"}],\
\"pop\":0},\
{\"dt\":1779919200,\
\"temp\":288.3,\
\"feels_like\":287.41,\
\"pressure\":1011,\
\"humidity\":59,\
\"dew_point\":281.51,\
\"uvi\":5.16,\
\"clouds\":42,\
\"visibility\":10000,\
\"wind_speed\":6.47,\
\"wind_deg\":237,\
\"wind_gust\":6.64,\
\"weather\":[{\"id\":802,\
\"main\":\"Clouds\",\
\"description\":\"scattered clouds\",\
\"icon\":\"03d\"}],\
\"pop\":0},\
{\"dt\":1779922800,\
\"temp\":288.22,\
\"feels_like\":287.38,\
\"pressure\":1011,\
\"humidity\":61,\
\"dew_point\":281.8,\
\"uvi\":4.14,\
\"clouds\":34,\
\"visibility\":10000,\
\"wind_speed\":5.64,\
\"wind_deg\":239,\
\"wind_gust\":5.77,\
\"weather\":[{\"id\":802,\
\"main\":\"Clouds\",\
\"description\":\"scattered clouds\",\
\"icon\":\"03d\"}],\
\"pop\":0},\
{\"dt\":1779926400,\
\"temp\":288.23,\
\"feels_like\":287.39,\
\"pressure\":1010,\
\"humidity\":61,\
\"dew_point\":281.94,\
\"uvi\":2.24,\
\"clouds\":31,\
\"visibility\":10000,\
\"wind_speed\":5.19,\
\"wind_deg\":250,\
\"wind_gust\":5.09,\
\"weather\":[{\"id\":802,\
\"main\":\"Clouds\",\
\"description\":\"scattered clouds\",\
\"icon\":\"03d\"}],\
\"pop\":0},\
{\"dt\":1779930000,\
\"temp\":288.06,\
\"feels_like\":287.23,\
\"pressure\":1011,\
\"humidity\":62,\
\"dew_point\":282.02,\
\"uvi\":0.92,\
\"clouds\":18,\
\"visibility\":10000,\
\"wind_speed\":5.4,\
\"wind_deg\":252,\
\"wind_gust\":5.21,\
\"weather\":[{\"id\":801,\
\"main\":\"Clouds\",\
\"description\":\"few clouds\",\
\"icon\":\"02d\"}],\
\"pop\":0},\
{\"dt\":1779933600,\
\"temp\":287.77,\
\"feels_like\":286.98,\
\"pressure\":1011,\
\"humidity\":65,\
\"dew_point\":282.35,\
\"uvi\":0.22,\
\"clouds\":28,\
\"visibility\":10000,\
\"wind_speed\":5.35,\
\"wind_deg\":255,\
\"wind_gust\":5.25,\
\"weather\":[{\"id\":802,\
\"main\":\"Clouds\",\
\"description\":\"scattered clouds\",\
\"icon\":\"03d\"}],\
\"pop\":0},\
{\"dt\":1779937200,\
\"temp\":287.42,\
\"feels_like\":286.65,\
\"pressure\":1011,\
\"humidity\":67,\
\"dew_point\":282.53,\
\"uvi\":0,\
\"clouds\":22,\
\"visibility\":10000,\
\"wind_speed\":5.6,\
\"wind_deg\":263,\
\"wind_gust\":5.52,\
\"weather\":[{\"id\":801,\
\"main\":\"Clouds\",\
\"description\":\"few clouds\",\
\"icon\":\"02n\"}],\
\"pop\":0},\
{\"dt\":1779940800,\
\"temp\":287.28,\
\"feels_like\":286.55,\
\"pressure\":1011,\
\"humidity\":69,\
\"dew_point\":282.9,\
\"uvi\":0,\
\"clouds\":17,\
\"visibility\":10000,\
\"wind_speed\":5.83,\
\"wind_deg\":274,\
\"wind_gust\":5.77,\
\"weather\":[{\"id\":801,\
\"main\":\"Clouds\",\
\"description\":\"few clouds\",\
\"icon\":\"02n\"}],\
\"pop\":0},\
{\"dt\":1779944400,\
\"temp\":287.2,\
\"feels_like\":286.49,\
\"pressure\":1012,\
\"humidity\":70,\
\"dew_point\":283.15,\
\"uvi\":0,\
\"clouds\":13,\
\"visibility\":10000,\
\"wind_speed\":5.62,\
\"wind_deg\":272,\
\"wind_gust\":5.59,\
\"weather\":[{\"id\":801,\
\"main\":\"Clouds\",\
\"description\":\"few clouds\",\
\"icon\":\"02n\"}],\
\"pop\":0},\
{\"dt\":1779948000,\
\"temp\":287.08,\
\"feels_like\":286.38,\
\"pressure\":1012,\
\"humidity\":71,\
\"dew_point\":283.13,\
\"uvi\":0,\
\"clouds\":11,\
\"visibility\":10000,\
\"wind_speed\":4.87,\
\"wind_deg\":277,\
\"wind_gust\":4.74,\
\"weather\":[{\"id\":801,\
\"main\":\"Clouds\",\
\"description\":\"few clouds\",\
\"icon\":\"02n\"}],\
\"pop\":0},\
{\"dt\":1779951600,\
\"temp\":287.07,\
\"feels_like\":286.37,\
\"pressure\":1013,\
\"humidity\":71,\
\"dew_point\":283.07,\
\"uvi\":0,\
\"clouds\":6,\
\"visibility\":10000,\
\"wind_speed\":4.46,\
\"wind_deg\":271,\
\"wind_gust\":4.48,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01n\"}],\
\"pop\":0},\
{\"dt\":1779955200,\
\"temp\":286.93,\
\"feels_like\":286.22,\
\"pressure\":1012,\
\"humidity\":71,\
\"dew_point\":283.14,\
\"uvi\":0,\
\"clouds\":14,\
\"visibility\":10000,\
\"wind_speed\":4.07,\
\"wind_deg\":269,\
\"wind_gust\":4.07,\
\"weather\":[{\"id\":801,\
\"main\":\"Clouds\",\
\"description\":\"few clouds\",\
\"icon\":\"02n\"}],\
\"pop\":0},\
{\"dt\":1779958800,\
\"temp\":286.85,\
\"feels_like\":286.13,\
\"pressure\":1012,\
\"humidity\":71,\
\"dew_point\":282.94,\
\"uvi\":0,\
\"clouds\":10,\
\"visibility\":10000,\
\"wind_speed\":2.62,\
\"wind_deg\":252,\
\"wind_gust\":2.55,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01n\"}],\
\"pop\":0},\
{\"dt\":1779962400,\
\"temp\":286.83,\
\"feels_like\":286.13,\
\"pressure\":1013,\
\"humidity\":72,\
\"dew_point\":283.14,\
\"uvi\":0,\
\"clouds\":13,\
\"visibility\":10000,\
\"wind_speed\":3.64,\
\"wind_deg\":268,\
\"wind_gust\":3.67,\
\"weather\":[{\"id\":801,\
\"main\":\"Clouds\",\
\"description\":\"few clouds\",\
\"icon\":\"02n\"}],\
\"pop\":0},\
{\"dt\":1779966000,\
\"temp\":286.81,\
\"feels_like\":286.11,\
\"pressure\":1013,\
\"humidity\":72,\
\"dew_point\":282.98,\
\"uvi\":0,\
\"clouds\":12,\
\"visibility\":10000,\
\"wind_speed\":2.54,\
\"wind_deg\":249,\
\"wind_gust\":2.77,\
\"weather\":[{\"id\":801,\
\"main\":\"Clouds\",\
\"description\":\"few clouds\",\
\"icon\":\"02n\"}],\
\"pop\":0},\
{\"dt\":1779969600,\
\"temp\":286.71,\
\"feels_like\":286,\
\"pressure\":1013,\
\"humidity\":72,\
\"dew_point\":282.89,\
\"uvi\":0,\
\"clouds\":13,\
\"visibility\":10000,\
\"wind_speed\":2.01,\
\"wind_deg\":248,\
\"wind_gust\":2.23,\
\"weather\":[{\"id\":500,\
\"main\":\"Rain\",\
\"description\":\"light rain\",\
\"icon\":\"10n\"}],\
\"pop\":0.2,\
\"rain\":{\"1h\":0.11}},\
{\"dt\":1779973200,\
\"temp\":286.67,\
\"feels_like\":285.96,\
\"pressure\":1013,\
\"humidity\":72,\
\"dew_point\":282.89,\
\"uvi\":0,\
\"clouds\":26,\
\"visibility\":10000,\
\"wind_speed\":1.53,\
\"wind_deg\":281,\
\"wind_gust\":1.55,\
\"weather\":[{\"id\":500,\
\"main\":\"Rain\",\
\"description\":\"light rain\",\
\"icon\":\"10d\"}],\
\"pop\":1,\
\"rain\":{\"1h\":0.19}}],\
\"daily\":[{\"dt\":1779822000,\
\"sunrise\":1779799567,\
\"sunset\":1779850503,\
\"moonrise\":1779836340,\
\"moonset\":1779789060,\
\"moon_phase\":0.36,\
\"summary\":\"You can expect partly cloudy in the morning,\
 with clearing in the afternoon\",\
\"temp\":{\"day\":290,\
\"min\":285.97,\
\"max\":291.42,\
\"night\":285.97,\
\"eve\":289.63,\
\"morn\":287.57},\
\"feels_like\":{\"day\":289.67,\
\"night\":285.08,\
\"eve\":289.19,\
\"morn\":287.21},\
\"pressure\":1011,\
\"humidity\":74,\
\"dew_point\":283.61,\
\"wind_speed\":8.25,\
\"wind_deg\":263,\
\"wind_gust\":8.04,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"clouds\":99,\
\"pop\":0,\
\"uvi\":7.89},\
{\"dt\":1779908400,\
\"sunrise\":1779885941,\
\"sunset\":1779936943,\
\"moonrise\":1779926220,\
\"moonset\":1779877020,\
\"moon_phase\":0.39,\
\"summary\":\"There will be partly cloudy today\",\
\"temp\":{\"day\":287.17,\
\"min\":286.22,\
\"max\":288.3,\
\"night\":287.08,\
\"eve\":288.06,\
\"morn\":286.22},\
\"feels_like\":{\"day\":286.32,\
\"night\":286.38,\
\"eve\":287.23,\
\"morn\":285.38},\
\"pressure\":1011,\
\"humidity\":65,\
\"dew_point\":281.83,\
\"wind_speed\":6.47,\
\"wind_deg\":237,\
\"wind_gust\":6.66,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"clouds\":100,\
\"pop\":0,\
\"uvi\":5.46},\
{\"dt\":1779994800,\
\"sunrise\":1779972317,\
\"sunset\":1780023382,\
\"moonrise\":1780016160,\
\"moonset\":1779965160,\
\"moon_phase\":0.42,\
\"summary\":\"Expect a day of partly cloudy with rain\",\
\"temp\":{\"day\":288.42,\
\"min\":286.67,\
\"max\":289.46,\
\"night\":287.76,\
\"eve\":289.09,\
\"morn\":286.67},\
\"feels_like\":{\"day\":287.67,\
\"night\":287.05,\
\"eve\":288.33,\
\"morn\":285.96},\
\"pressure\":1014,\
\"humidity\":64,\
\"dew_point\":282.68,\
\"wind_speed\":4.92,\
\"wind_deg\":249,\
\"wind_gust\":4.77,\
\"weather\":[{\"id\":500,\
\"main\":\"Rain\",\
\"description\":\"light rain\",\
\"icon\":\"10d\"}],\
\"clouds\":0,\
\"pop\":1,\
\"rain\":0.54,\
\"uvi\":7.89},\
{\"dt\":1780081200,\
\"sunrise\":1780058695,\
\"sunset\":1780109820,\
\"moonrise\":1780106160,\
\"moonset\":1780053420,\
\"moon_phase\":0.45,\
\"summary\":\"Expect a day of partly cloudy with clear spells\",\
\"temp\":{\"day\":288.57,\
\"min\":287.11,\
\"max\":289.4,\
\"night\":288.3,\
\"eve\":289.32,\
\"morn\":287.11},\
\"feels_like\":{\"day\":287.86,\
\"night\":287.88,\
\"eve\":288.77,\
\"morn\":286.42},\
\"pressure\":1016,\
\"humidity\":65,\
\"dew_point\":283.07,\
\"wind_speed\":7.63,\
\"wind_deg\":265,\
\"wind_gust\":7.76,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01d\"}],\
\"clouds\":1,\
\"pop\":0,\
\"uvi\":9.7},\
{\"dt\":1780167600,\
\"sunrise\":1780145073,\
\"sunset\":1780196258,\
\"moonrise\":1780196040,\
\"moonset\":1780141980,\
\"moon_phase\":0.48,\
\"summary\":\"There will be clear sky today\",\
\"temp\":{\"day\":289.44,\
\"min\":288.04,\
\"max\":290.58,\
\"night\":288.68,\
\"eve\":290.45,\
\"morn\":288.04},\
\"feels_like\":{\"day\":288.98,\
\"night\":288.38,\
\"eve\":290.17,\
\"morn\":287.65},\
\"pressure\":1015,\
\"humidity\":71,\
\"dew_point\":285.39,\
\"wind_speed\":5.67,\
\"wind_deg\":260,\
\"wind_gust\":5.96,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01d\"}],\
\"clouds\":4,\
\"pop\":0,\
\"uvi\":10},\
{\"dt\":1780254000,\
\"sunrise\":1780231454,\
\"sunset\":1780282695,\
\"moonrise\":1780285800,\
\"moonset\":1780230900,\
\"moon_phase\":0.5,\
\"summary\":\"There will be clear sky today\",\
\"temp\":{\"day\":289.98,\
\"min\":288.61,\
\"max\":291.48,\
\"night\":289.3,\
\"eve\":291.23,\
\"morn\":288.61},\
\"feels_like\":{\"day\":289.7,\
\"night\":288.93,\
\"eve\":290.92,\
\"morn\":288.4},\
\"pressure\":1014,\
\"humidity\":76,\
\"dew_point\":286.81,\
\"wind_speed\":4.87,\
\"wind_deg\":258,\
\"wind_gust\":5.51,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01d\"}],\
\"clouds\":0,\
\"pop\":0,\
\"uvi\":10},\
{\"dt\":1780340400,\
\"sunrise\":1780317836,\
\"sunset\":1780369131,\
\"moonrise\":1780375320,\
\"moonset\":1780320120,\
\"moon_phase\":0.54,\
\"summary\":\"There will be partly cloudy today\",\
\"temp\":{\"day\":290.56,\
\"min\":288.81,\
\"max\":292.14,\
\"night\":289.82,\
\"eve\":291.95,\
\"morn\":289},\
\"feels_like\":{\"day\":290.26,\
\"night\":289.53,\
\"eve\":291.64,\
\"morn\":288.81},\
\"pressure\":1015,\
\"humidity\":73,\
\"dew_point\":286.53,\
\"wind_speed\":5.51,\
\"wind_deg\":268,\
\"wind_gust\":7.18,\
\"weather\":[{\"id\":804,\
\"main\":\"Clouds\",\
\"description\":\"overcast clouds\",\
\"icon\":\"04d\"}],\
\"clouds\":100,\
\"pop\":0,\
\"uvi\":10},\
{\"dt\":1780426800,\
\"sunrise\":1780404219,\
\"sunset\":1780455566,\
\"moonrise\":1780464420,\
\"moonset\":1780409760,\
\"moon_phase\":0.57,\
\"summary\":\"There will be clear sky today\",\
\"temp\":{\"day\":291.46,\
\"min\":288.86,\
\"max\":293.32,\
\"night\":290.36,\
\"eve\":293.2,\
\"morn\":288.86},\
\"feels_like\":{\"day\":291.07,\
\"night\":290.15,\
\"eve\":292.93,\
\"morn\":288.55},\
\"pressure\":1013,\
\"humidity\":66,\
\"dew_point\":285.64,\
\"wind_speed\":5.3,\
\"wind_deg\":266,\
\"wind_gust\":7.12,\
\"weather\":[{\"id\":800,\
\"main\":\"Clear\",\
\"description\":\"clear sky\",\
\"icon\":\"01d\"}],\
\"clouds\":1,\
\"pop\":0,\
\"uvi\":10}]}";

const char *OwmAirPollutionTestResponse = 
"{\"coord\":{\"lon\":-118.3364,\
\"lat\":33.7647},\
\"list\":[{\"main\":{\"aqi\":2},\
\"components\":{\"co\":124.5,\
\"no\":0.13,\
\"no2\":0.62,\
\"o3\":98.73,\
\"so2\":0.8,\
\"pm2_5\":9.74,\
\"pm10\":23.45,\
\"nh3\":0},\
\"dt\":1779300000},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":124.07,\
\"no\":0.1,\
\"no2\":0.48,\
\"o3\":99.51,\
\"so2\":0.78,\
\"pm2_5\":9.72,\
\"pm10\":23.71,\
\"nh3\":0},\
\"dt\":1779303600},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":123.23,\
\"no\":0.08,\
\"no2\":0.39,\
\"o3\":99.62,\
\"so2\":0.76,\
\"pm2_5\":9.64,\
\"pm10\":23.98,\
\"nh3\":0},\
\"dt\":1779307200},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":122.11,\
\"no\":0.07,\
\"no2\":0.34,\
\"o3\":99.58,\
\"so2\":0.75,\
\"pm2_5\":9.6,\
\"pm10\":24.33,\
\"nh3\":0},\
\"dt\":1779310800},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":121.07,\
\"no\":0.07,\
\"no2\":0.32,\
\"o3\":99.97,\
\"so2\":0.76,\
\"pm2_5\":9.61,\
\"pm10\":24.84,\
\"nh3\":0},\
\"dt\":1779314400},\
{\"main\":{\"aqi\":3},\
\"components\":{\"co\":119.74,\
\"no\":0.06,\
\"no2\":0.33,\
\"o3\":100.68,\
\"so2\":0.79,\
\"pm2_5\":9.71,\
\"pm10\":25.53,\
\"nh3\":0},\
\"dt\":1779318000},\
{\"main\":{\"aqi\":3},\
\"components\":{\"co\":118.27,\
\"no\":0.06,\
\"no2\":0.36,\
\"o3\":101.53,\
\"so2\":0.82,\
\"pm2_5\":9.86,\
\"pm10\":26.2,\
\"nh3\":0},\
\"dt\":1779321600},\
{\"main\":{\"aqi\":3},\
\"components\":{\"co\":117.11,\
\"no\":0.05,\
\"no2\":0.42,\
\"o3\":102.49,\
\"so2\":0.85,\
\"pm2_5\":9.97,\
\"pm10\":26.52,\
\"nh3\":0},\
\"dt\":1779325200},\
{\"main\":{\"aqi\":3},\
\"components\":{\"co\":116.45,\
\"no\":0.03,\
\"no2\":0.53,\
\"o3\":101.77,\
\"so2\":0.87,\
\"pm2_5\":10.06,\
\"pm10\":26.79,\
\"nh3\":0},\
\"dt\":1779328800},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":116.61,\
\"no\":0,\
\"no2\":0.64,\
\"o3\":99.43,\
\"so2\":0.89,\
\"pm2_5\":10.18,\
\"pm10\":27.05,\
\"nh3\":0},\
\"dt\":1779332400},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":116.97,\
\"no\":0,\
\"no2\":0.66,\
\"o3\":96.85,\
\"so2\":0.89,\
\"pm2_5\":10.17,\
\"pm10\":26.71,\
\"nh3\":0.01},\
\"dt\":1779336000},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":115.41,\
\"no\":0,\
\"no2\":0.66,\
\"o3\":94.41,\
\"so2\":0.86,\
\"pm2_5\":9.72,\
\"pm10\":25.43,\
\"nh3\":0.01},\
\"dt\":1779339600},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":113.14,\
\"no\":0,\
\"no2\":0.67,\
\"o3\":92.49,\
\"so2\":0.82,\
\"pm2_5\":9.07,\
\"pm10\":23.73,\
\"nh3\":0},\
\"dt\":1779343200},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":110.89,\
\"no\":0,\
\"no2\":0.67,\
\"o3\":90.87,\
\"so2\":0.78,\
\"pm2_5\":8.38,\
\"pm10\":21.9,\
\"nh3\":0},\
\"dt\":1779346800},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":109.16,\
\"no\":0,\
\"no2\":0.68,\
\"o3\":89.21,\
\"so2\":0.76,\
\"pm2_5\":7.87,\
\"pm10\":20.63,\
\"nh3\":0},\
\"dt\":1779350400},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":107.71,\
\"no\":0,\
\"no2\":0.69,\
\"o3\":87.18,\
\"so2\":0.75,\
\"pm2_5\":7.51,\
\"pm10\":19.85,\
\"nh3\":0},\
\"dt\":1779354000},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":106.74,\
\"no\":0,\
\"no2\":0.72,\
\"o3\":84.57,\
\"so2\":0.75,\
\"pm2_5\":7.29,\
\"pm10\":19.53,\
\"nh3\":0},\
\"dt\":1779357600},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":105.86,\
\"no\":0,\
\"no2\":0.74,\
\"o3\":81.36,\
\"so2\":0.75,\
\"pm2_5\":7.2,\
\"pm10\":19.69,\
\"nh3\":0},\
\"dt\":1779361200},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":105.18,\
\"no\":0,\
\"no2\":0.76,\
\"o3\":78.12,\
\"so2\":0.77,\
\"pm2_5\":7.25,\
\"pm10\":20.24,\
\"nh3\":0},\
\"dt\":1779364800},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":104.97,\
\"no\":0,\
\"no2\":0.78,\
\"o3\":75.38,\
\"so2\":0.79,\
\"pm2_5\":7.39,\
\"pm10\":21.07,\
\"nh3\":0},\
\"dt\":1779368400},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":105.07,\
\"no\":0.01,\
\"no2\":0.87,\
\"o3\":73.21,\
\"so2\":0.8,\
\"pm2_5\":7.59,\
\"pm10\":22.08,\
\"nh3\":0},\
\"dt\":1779372000},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":105.62,\
\"no\":0.08,\
\"no2\":0.87,\
\"o3\":72.31,\
\"so2\":0.82,\
\"pm2_5\":7.83,\
\"pm10\":23.16,\
\"nh3\":0},\
\"dt\":1779375600},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":106.25,\
\"no\":0.14,\
\"no2\":0.75,\
\"o3\":73.81,\
\"so2\":0.84,\
\"pm2_5\":8.01,\
\"pm10\":24.03,\
\"nh3\":0},\
\"dt\":1779379200},\
{\"main\":{\"aqi\":2},\
\"components\":{\"co\":107.38,\
\"no\":0.14,\
\"no2\":0.62,\
\"o3\":76.69,\
\"so2\":0.84,\
\"pm2_5\":8.28,\
\"pm10\":25.11,\
\"nh3\":0},\
\"dt\":1779382800}]}";

void TestOwm(TFT_eSprite &spr,bool bHighRes) 
{
   OwmArgs Args;

// City name that will be shown in the top-right corner of the display.
   Args.City = "Rancho Palos Verdes";
   Args.TimeFormat = "%l:%M %P";
   Args.DateFormat = "%a, %B %e";
   Args.ForecastApiResponse = OwmForecastTestResponse;
   Args.AirPollutionApiResponse = OwmAirPollutionTestResponse;
   Args.inTemp     = NAN;
   Args.inHumidity = NAN;
   Args.batteryVoltage = 2960;
   Args.Rssi = -59;
   Args.bMetric = false;
   Args.bHighRes = bHighRes;

   LOG("Owm test\n");

   Args.WindSpeed = Args.bMetric ? UNITS_SPEED_KILOMETERSPERHOUR : 
                  UNITS_SPEED_MILESPERHOUR;
   Args.DistanceType = Args.bMetric ? UNITS_DIST_KILOMETERS : UNITS_DIST_MILES;
   Args.PrecipType = Args.bMetric ? UNITS_DAILY_PRECIP_MILLIMETERS : 
                                    UNITS_DAILY_PRECIP_INCHES;

   Args.PrecipHrType = Args.bMetric ? UNITS_HOURLY_PRECIP_MILLIMETERS :
                                      UNITS_HOURLY_PRECIP_INCHES;
   Args.PressureType = Args.bMetric ? UNITS_PRES_MILLIBARS :
                                      UNITS_PRES_INCHESOFMERCURY;

   Args.bDisplayAlerts = Args.bMetric ? false : true;
   Args.bDisplayAlerts = Args.bMetric ? false : true;
   if(Args.bHighRes) {
      Args.DisplayWidth    = 800;
      Args.DisplayHeight   = 480;
      Args.PosSunrise      = 0;
      Args.PosSunset       = 1;
      Args.PosWind         = 2;
      Args.PosHumidity     = 3;
      Args.PosUvi          = 4;
      Args.PosPressure     = 5;
      Args.PosAirQuality   = 6;
      Args.PosVisibility   = 7;
      Args.PosIntemp       = 8;
      Args.PosInhumidity   = 9;
      Args.PosMoonrise     = -1;
      Args.PosMoonset      = -1;
      Args.PosMoonphase    = -1;
      Args.PosDewpoint     = -1;
   }
   else {
  // if a 640 x 384 display is used, then positions 6,7,8,9 are not available
      Args.DisplayWidth    = 640;
      Args.DisplayHeight   = 384;
      Args.PosSunrise      = 0;
      Args.PosSunset       = 1;
      Args.PosWind         = 2;
      Args.PosHumidity     = 3;
      Args.PosVisibility   = 4;
      Args.PosIntemp       = 5;
      Args.PosUvi          = -1;
      Args.PosPressure     = -1;
      Args.PosAirQuality   = -1;
      Args.PosInhumidity   = -1;
      Args.PosMoonrise     = -1;
      Args.PosMoonset      = -1;
      Args.PosMoonphase    = -1;
      Args.PosDewpoint     = -1;
   }

   class DrawOWM *owm = new DrawOWM(spr,Args);
   owm->DrawIt();
   delete owm;
}



