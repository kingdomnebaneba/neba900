#include <QDateTime>
#include <stdio.h>
#include <string.h>

#include <CCalendar.h>
#include <CMulticalendar.h>
#include <CEvent.h>
#include <CAlarm.h>

#define CALENDAR_NAME "Org-calendar"
#define TIMEZONE_OFFSET -3600*5

using namespace std;

const string clean_string(const string &str_orig) {
  int beg = str_orig.find_first_not_of(" ");
  int end = str_orig.find_last_not_of(" ");
  return str_orig.substr(beg, end-beg+1);
}

const QDateTime parse_time_string(const string &cleaned) {
  int year  = atoi(cleaned.substr(0,4).c_str());
  int month = atoi(cleaned.substr(5,7).c_str());
  int day   = atoi(cleaned.substr(8,10).c_str());
  int hour  = atoi(cleaned.substr(11,13).c_str());
  int min   = atoi(cleaned.substr(14,16).c_str());
  int sec   = atoi(cleaned.substr(17,19).c_str());

  QDate inp_date = QDate(year, month, day);
  QTime inp_time = QTime(hour, min, sec);

  //QDateTime datetime = QDateTime::currentDateTime();
  return QDateTime(inp_date, inp_time);
}

void announce_time(QDateTime &dt, const string &duration) {
  QDate date = dt.date();
  QTime time = dt.time();
  printf("<< adding alarm: %02d-%02d-%02d %02d:%02d:%02d, alarm: %s >>\n", date.year(), date.month(), date.day(),
         time.hour(), time.minute(), time.second(), duration.c_str());
}

int main(int argc, char *argv[])
{
  if(argc > 1) {
    
    string eventName = "";
    string eventDesc = "";
    string eventLoc = "";
    
    string str_start_time = "";
    string str_end_time = "";
    string str_duration = "";

    string calendar_name = CALENDAR_NAME;

    for(int i = 0; i < argc; ++i) {
      string arg = argv[i];
      if(arg.substr(0, 2) == "--") {
        if(arg == "--cal") {
          calendar_name = clean_string(argv[i+1]);
        } else if(arg == "--start") {
          str_start_time = clean_string(argv[i+1]);
        } else if(arg == "--end") {
          str_end_time = clean_string(argv[i+1]);
        } else if(arg == "--name") {
          eventName = clean_string(argv[i+1]);
        } else if(arg == "--desc") {
          eventDesc = clean_string(argv[i+1]);
        } else if(arg == "--loc") {
          eventLoc = clean_string(argv[i+1]);
        } else if(arg == "--alarm") {
          str_duration = clean_string(argv[i+1]);
        }
      }
    } // for int i

    int timestamp_start = -1;
    QDateTime qdt_start;
    if( str_start_time.length() == 19 ) {
      //QDateTime datetime = 
      QDateTime qdt_start = parse_time_string(str_start_time); //QDateTime(datetime.date(), datetime.time());
      // qdt_start = qdt_start.addSecs( 60 * 15 + 30);
      //qdt_start = qdt_start.addSecs(TIMEZONE_OFFSET);
      timestamp_start = qdt_start.toTime_t();
    }

    int timestamp_end = timestamp_start + 3600;
    if( str_end_time.length() == 19 ) {
      QDateTime qdt_end = parse_time_string(str_end_time);
      //qdt_end = qdt_end.addSecs(TIMEZONE_OFFSET);
      timestamp_end = qdt_end.toTime_t();
    }

    int duration_before = E_AM_15MIN;
    int trigger_duration = 0;
    if(str_duration.length()) {
      if(str_duration == "none") {
        duration_before = E_AM_NONE;
      } else if(str_duration == "exact") {
        duration_before = E_AM_ETIME;
      } else if(str_duration == "5min") {
        duration_before = E_AM_5MIN;
        trigger_duration = 5*60;
      } else if(str_duration == "15min") {
        duration_before = E_AM_15MIN;
        trigger_duration = 15*60;
      } else if(str_duration == "30min") {
        duration_before = E_AM_30MIN;
        trigger_duration = 30*60;
      } else if(str_duration == "1hr") {
        duration_before = E_AM_1HR;
        trigger_duration = 3600;
      } else if(str_duration == "3hr") {
        duration_before = E_AM_3HR;
        trigger_duration = 3*3600;
      } else if(str_duration == "1day") {
        duration_before = E_AM_DAYBEFORE;
        trigger_duration = 24*3600;
      }
    }

    if(timestamp_start != -1) {
      announce_time(qdt_start, str_duration);

      CMulticalendar *multiCalendar = CMulticalendar::MCInstance();

      int errorCode = 0;
      CCalendar *calendar = multiCalendar->getCalendarByName(calendar_name, errorCode);
      CEvent *pEvent = new CEvent(eventName,       // summary
                                  eventDesc,       // description
                                  eventLoc,        // location
                                  timestamp_start, // start time
                                  timestamp_end    // end time
                                  );
      if(duration_before > 0) {
        CAlarm *alarmEvent = new CAlarm();
        alarmEvent->setDuration(duration_before);
        alarmEvent->setTrigger(timestamp_start - trigger_duration);
        pEvent->setAlarm(alarmEvent);
        calendar->addEvent(pEvent, errorCode);
        delete alarmEvent;
        delete pEvent;
      }
      delete calendar;
      delete multiCalendar;
      
    }
  }

  return 0;
}
