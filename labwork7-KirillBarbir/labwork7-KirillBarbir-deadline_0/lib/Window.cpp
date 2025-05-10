#include "Window.h"
#include "Backend.h"

ftxui::Element CreateTimeBox(nlohmann::json weather_json, int day_number, int time) {
    nlohmann::json hourly = weather_json["hourly"];
    ftxui::Element timestamp_box = ftxui::vbox(
        {
            //ftxui::text("time: " + hourly["time_"][day_number * kHoursInDay + time].dump()),
            ftxui::text("temperature: " + hourly["temperature_2m"][day_number * kHoursInDay + time].dump()),
            ftxui::text("cloud cover: " + hourly["cloud_cover"][day_number * kHoursInDay + time].dump()),
            ftxui::text("precipitation: " + hourly["precipitation"][day_number * kHoursInDay + time].dump()),
            /*ftxui::text("probability: " + (
            (hourly["precipitation_probability"][day_number * kHoursInDay + time].dump()) != "null" ?
                hourly["precipitation_probability"][day_number * kHoursInDay + time].dump()
                : "no info")),*/
            ftxui::text("humidity: " + hourly["relative_humidity_2m"][day_number * kHoursInDay + time].dump()),
            ftxui::text("weather_code: " + hourly["weather_code"][day_number * kHoursInDay + time].dump()),
            ftxui::text("Wind: " + hourly["wind_speed_10m"][day_number * kHoursInDay + time].dump() + ", "
                            + hourly["wind_direction_10m"][day_number * kHoursInDay + time].dump())
        }) | ftxui::border;
    return timestamp_box;
}

ftxui::Element CreateDayBox(nlohmann::json weather_json, int day_number) {
    ftxui::Element day_box = ftxui::vbox(
        {ftxui::text(weather_json["hourly"]["time"][day_number * kHoursInDay].dump().substr(1, kDateStringLength))
            | ftxui::center,
            ftxui::hbox(
                {
                    CreateTimeBox(weather_json, day_number, kNightHour),
                    ftxui::separatorEmpty(),
                    CreateTimeBox(weather_json, day_number, kMorningHour),
                    ftxui::separatorEmpty(),
                    CreateTimeBox(weather_json, day_number, kAfternoonHour),
                    ftxui::separatorEmpty(),
                    CreateTimeBox(weather_json, day_number, kEveningHour)
                })
        });

    return day_box;
}

ftxui::Element CreatePage(nlohmann::json weather_json, int days_amount, int current_page, int pages_amount) {
    if (pages_amount == current_page + 1) {
        --days_amount;
        days_amount %= kDaysPerPage;
        ++days_amount;
    }
    days_amount = std::min(days_amount, kDaysPerPage);
    ftxui::Element days_boxes = ftxui::vbox(
        {
            (days_amount > 1) ? CreatePage(weather_json,
                                           days_amount - 1,
                                           current_page,
                                           pages_amount) : ftxui::emptyElement(),
            CreateDayBox(weather_json, days_amount - 1 + current_page * kDaysPerPage)
        });
    return days_boxes;
}

void DisplayWeather(nlohmann::json config) {
    auto enter_time = std::chrono::steady_clock::now();
    std::mutex thread_safe;
    std::vector<nlohmann::json> weather_jsons = GetWeathers(config);
    int city_index = 0;
    int cities_amount = config["city"].size();
    int days_amount = config["days_amount"];
    int current_page = 0;
    int pages_amount = (days_amount - 1) / kDaysPerPage + 1;
    auto renderer = ftxui::Renderer([&] {
      thread_safe.lock();
      ftxui::Element output_box = ftxui::vbox(
          {
              ftxui::text("city: " + config["city"][city_index].dump()) | ftxui::center,
              CreatePage(weather_jsons[city_index], days_amount, current_page, pages_amount) | ftxui::center,
              ftxui::text("time_elapsed: " + std::to_string(static_cast<long long>(duration_cast<std::chrono::seconds>(
                  std::chrono::steady_clock::now() - enter_time).count())) + " s"),
              ftxui::text("page: " + std::to_string(current_page + 1) + " out of " + std::to_string(pages_amount))
                  | ftxui::align_right
          });
      thread_safe.unlock();
      return output_box;
    });

    ftxui::ScreenInteractive main_screen = ftxui::ScreenInteractive::Fullscreen();
    auto press = ftxui::CatchEvent(renderer, [&](ftxui::Event event) {
      if (event == ftxui::Event::Escape) {
          main_screen.ExitLoopClosure()();
          return true;
      }

      if (event == ftxui::Event::Character('n')) {
          city_index += cities_amount - 1;
          city_index %= cities_amount;
          return true;
      }

      if (event == ftxui::Event::Character('p')) {
          ++city_index;
          city_index %= cities_amount;
          return true;
      }

      if (event == ftxui::Event::Character('+')) {
          if (days_amount < 16) {
              ++days_amount;
          }
          pages_amount = (days_amount - 1) / kDaysPerPage + 1;
          current_page = std::min(current_page, pages_amount - 1);
          return true;
      }

      if (event == ftxui::Event::Character('-')) {
          if (days_amount > 1) {
              --days_amount;
          }
          pages_amount = (days_amount - 1) / kDaysPerPage + 1;
          current_page = std::min(current_page, pages_amount - 1);
          return true;
      }

      if (event == ftxui::Event::Character('z')) {
          current_page += pages_amount - 1;
          current_page %= pages_amount;
          return true;
      }

      if (event == ftxui::Event::Character('x')) {
          ++current_page;
          current_page %= pages_amount;
          return true;
      }

      return false;
    });

    std::thread update([&main_screen, &weather_jsons, &config, &enter_time, &thread_safe]() {
      while (true) {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          if ((duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - enter_time).count())
              % stoi(config["frequency"].dump()) == 0) {
              thread_safe.lock();
              weather_jsons.clear();
              weather_jsons = GetWeathers(config);
              main_screen.Post(ftxui::Event::Custom);
              thread_safe.unlock();
          }
      }
    });

    update.detach();
    main_screen.Loop(press);
}