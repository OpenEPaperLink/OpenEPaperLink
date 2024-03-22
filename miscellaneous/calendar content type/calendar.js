function getEventsForNextDay(days) {
  var start = new Date();
  start.setHours(0, 0, 0, 0);
  var end = new Date();
  if (days == undefined) days = 1;
  end.setDate(end.getDate() + parseInt(days, 10));
  var calendars = CalendarApp.getAllCalendars();
  var events = [];
  for (var i = 0; i < calendars.length; i++) {
    var calendar = calendars[i];
    var eventsInCalendar = calendar.getEvents(start, end);
    for (var j = 0; j < eventsInCalendar.length; j++) {
      var event = eventsInCalendar[j];
      events.push({
        calendar: i,
        title: event.getTitle(),
        start: Math.floor(event.getStartTime().getTime() / 1000),
        end: Math.floor(event.getEndTime().getTime() / 1000),
        isallday: event.isAllDayEvent()
      });
    }
  }
  events.sort(function(a, b) {
    return a.start - b.start;
  });
  return JSON.stringify(events);
}

function doGet(e) {
  if(!e) { 
    e = {parameter: {days: 1}};
  }
  const params = e.parameter;
  var content = getEventsForNextDay(params.days);
  var output = ContentService.createTextOutput(content);
  output.setMimeType(ContentService.MimeType.JSON);
  return output;
}
