#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]) try {
  rs2::colorizer color_map;

  rs2::config cfg;
  cfg.enable_stream(RS2_STREAM_COLOR);
  cfg.enable_stream(RS2_STREAM_DEPTH);

  rs2::pipeline pipe;
  pipe.start(cfg);

  rs2::align align_to_depth(RS2_STREAM_DEPTH);

  cv::namedWindow("color", CV_WINDOW_AUTOSIZE);
  cv::namedWindow("depth", CV_WINDOW_AUTOSIZE);

  while (true) {
    rs2::frameset frameset = pipe.wait_for_frames();
    frameset = align_to_depth.process(frameset);

    rs2::frame color = frameset.get_color_frame();
    rs2::frame depth = frameset.get_depth_frame().apply_filter(color_map);

    const int color_w = color.as<rs2::video_frame>().get_width();
    const int color_h = color.as<rs2::video_frame>().get_height();
    std::cout << "[color] width: " << color_w << " height: " << color_h
              << std::endl;

    const int depth_w = depth.as<rs2::video_frame>().get_width();
    const int depth_h = depth.as<rs2::video_frame>().get_height();
    std::cout << "[depth] width: " << depth_w << " height: " << depth_h
              << std::endl;

    cv::Mat color_image(cv::Size(color_w, color_h), CV_8UC3,
                        (void *)color.get_data(), cv::Mat::AUTO_STEP);
    cv::Mat depth_image(cv::Size(depth_w, depth_h), CV_8UC3,
                        (void *)depth.get_data(), cv::Mat::AUTO_STEP);
    cv::imshow("color", color_image);
    cv::imshow("depth", depth_image);
    cv::waitKey(60);
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