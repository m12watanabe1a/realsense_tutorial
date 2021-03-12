#include <librealsense2/rs.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>

int main(int argc, char *argv[]) try {
  rs2::colorizer color_map;
  rs2::config cfg;
  cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30);
  cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 30);

  rs2::pipeline pipe;
  pipe.start(cfg);
  cv::namedWindow("color");
  cv::namedWindow("depth");

  while (true) {
    rs2::frameset data = pipe.wait_for_frames();
    rs2::frame color = data.get_color_frame();
    rs2::frame depth = data.get_depth_frame().apply_filter(color_map);
    const int w = depth.as<rs2::video_frame>().get_width();
    const int h = depth.as<rs2::video_frame>().get_height();
    std::cout << "width: " << w << " height: " << h << std::endl;

    cv::Mat color_image(cv::Size(w, h), CV_8UC3, (void *)color.get_data(),
                        cv::Mat::AUTO_STEP);
    cv::Mat depth_image(cv::Size(w, h), CV_8UC3, (void *)depth.get_data(),
                        cv::Mat::AUTO_STEP);
    cv::imshow("color", color_image);
    cv::imshow("depth", depth_image);
    cv::waitKey(1);
  }
  return EXIT_SUCCESS;
} catch (const rs2::error &e) {
  std::cerr << "RealSense error calling " << e.get_failed_function() << "("
            << e.get_failed_args() << "):\n    " << e.what() << std::endl;
  return EXIT_FAILURE;
} catch (const std::exception &e) {
  std::cerr << e.what() << std::endl;
  return EXIT_FAILURE;
}