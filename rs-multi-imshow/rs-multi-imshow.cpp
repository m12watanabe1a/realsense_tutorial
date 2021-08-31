#include <iostream>
#include <string>
#include <librealsense2/rs.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <map>
#include <vector>

int main(int argc, char *argv[])
try
{
  rs2::context ctx;
  std::vector<std::string> serials;

  std::map<std::string, rs2::colorizer> colorizers;

  for (auto &&dev : ctx.query_devices())
  {
    serials.push_back(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
  }

  std::vector<rs2::pipeline> pipelines;
  for (auto &&serial : serials)
  {
    rs2::pipeline pipe(ctx);
    std::cout << "Device Serial: " << serial << std::endl;
    rs2::config cfg;
    cfg.enable_device(serial);
    cfg.enable_stream(RS2_STREAM_COLOR, 848, 480, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH, 848, 480, RS2_FORMAT_Z16, 30);
    pipe.start(cfg);
    pipelines.emplace_back(pipe);
    colorizers[serial] = rs2::colorizer();

    // prepare window to display
    cv::namedWindow(serial + " : color");
    cv::namedWindow(serial + " : depth");
  }

  while (true)
  {
    for (auto && pipe : pipelines)
    {
      rs2::frameset data = pipe.wait_for_frames();
      rs2::frame color = data.get_color_frame();

      std::string serial = rs2::sensor_from_frame(color)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
      rs2::frame depth = data.get_depth_frame();
      rs2::video_frame colorized_depth = colorizers[serial].colorize(depth);

      const int w = depth.as<rs2::video_frame>().get_width();
      const int h = depth.as<rs2::video_frame>().get_height();

      cv::Mat color_image(cv::Size(w, h), CV_8UC3, (void *)color.get_data(),
                          cv::Mat::AUTO_STEP);
      cv::Mat depth_image(cv::Size(w, h), CV_8UC3, (void *)colorized_depth.get_data(),
                          cv::Mat::AUTO_STEP);
      cv::imshow(serial + " : color", color_image);
      cv::imshow(serial + " : depth", depth_image);
      cv::waitKey(1);
    }
  }
  return EXIT_SUCCESS;
}
catch (const rs2::error &e)
{
  std::cerr << "RealSense error calling " << e.get_failed_function() << "("
            << e.get_failed_args() << "):\n    " << e.what() << std::endl;
  return EXIT_FAILURE;
}
catch (const std::exception &e)
{
  std::cerr << e.what() << std::endl;
  return EXIT_FAILURE;
}