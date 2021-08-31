#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]) try {
  rs2::colorizer color_map;

  rs2::config cfg;
  cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30);
  cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 30);

  rs2::pipeline pipe;

  cv::namedWindow("color");
  cv::namedWindow("depth");

  auto profile = pipe.start(cfg);
  auto depth_stream =
      profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
  auto depth_intr = depth_stream.get_intrinsics();

  while (true) {
    rs2::frameset frameset = pipe.wait_for_frames();

    rs2::align align(RS2_STREAM_COLOR);
    rs2::frameset aligned_frames = align.process(frameset);

    rs2::video_frame color_frame = aligned_frames.first(RS2_STREAM_COLOR);
    rs2::depth_frame depth_frame = aligned_frames.get_depth_frame();
    rs2::video_frame colorized_depth = color_map.colorize(depth_frame);

    const int color_w = color_frame.as<rs2::video_frame>().get_width();
    const int color_h = color_frame.as<rs2::video_frame>().get_height();

    const int depth_w = colorized_depth.as<rs2::video_frame>().get_width();
    const int depth_h = colorized_depth.as<rs2::video_frame>().get_height();
    cv::Mat color_image(cv::Size(color_w, color_h), CV_8UC3,
                        (void *)color_frame.get_data(), cv::Mat::AUTO_STEP);
    cv::Mat depth_image(cv::Size(depth_w, depth_h), CV_8UC3,
                        (void *)colorized_depth.get_data(), cv::Mat::AUTO_STEP);

    cv::Point dot_P;

    // Draw Circle
    dot_P.x = 300;
    dot_P.y = 400;
    cv::Scalar dot_S(0, 0, 255);
    cv::circle(color_image, dot_P, 10, dot_S, -1, cv::LINE_AA);

    float pixel[2] = {float(dot_P.x), float(dot_P.y)};
    float depth = depth_frame.get_distance(pixel[0], pixel[1]);
    float point[3];
    rs2_deproject_pixel_to_point(point, &depth_intr, pixel, depth);

    std::cout << "depth: " << depth << std::endl;

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
