#ifndef __HEXAGON_SETTING_WINDOW__
#define __HEXAGON_SETTING_WINDOW__

#include <opencv2/core.hpp>

class HexagonSettingWindow
{
public:
	HexagonSettingWindow();
	HexagonSettingWindow(const cv::Mat& image, const std::string& window_name);
	~HexagonSettingWindow();

	void Open(const cv::Mat& image, const std::string& window_name);
	void Close();
	void setMaxWindowSize(const cv::Size& win_size);
	void init();
	int GetWindowKey() const;
	void GetHexagon(std::vector<cv::Point2f>& hexagon) const;

private:
	std::vector<cv::Point2f> _display_hexagon;
	static void _GenerateHexagon(std::vector<cv::Point2f>& hexagon, const cv::Point2f& center, float scale = 1, float start_rad = 0);
	static void _MoveHexagon(std::vector<cv::Point2f>& hexagon, const cv::Point2f& center);
	static double _CalcRotation(const cv::Point2f& pt1, const cv::Point2f& origin, int corner_id);

	cv::Size _max_window_size;
	float _display_scale;
	std::string _window_name;
	cv::Mat _src_image;
	cv::Mat _display_image;

	static float _ComputeDisplayScale(const cv::Size& image_size, const cv::Size& max_image_size);

	// Mouse Operating Function
	static void on_mouse(int event, int x, int y, int flag, void * param);
	void MouseButtonDown(int x, int y);
	void MouseDrag(int x, int y);
	void MouseButtonUp();
	void MouseMove(int x, int y);
	int _selected_corner_id;
	float _select_distance_threshold;

	// Draw Functions
	float _conrner_circle_radius;
	static void _DrawHex(cv::Mat& img, const std::vector<cv::Point2f>& hex, const cv::Scalar& color, int thickness = 1, int corner_circle_size = 0);
	void RedrawImage() const;
};

#endif

