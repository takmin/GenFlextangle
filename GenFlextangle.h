#ifndef __GEN_FLEXTANGLE__
#define __GEN_FLEXTANGLE__

#include <opencv2/imgproc.hpp>


class GenFlextangle
{
public:
	GenFlextangle();
	GenFlextangle(int output_width, const cv::Point& offset) {
		Initialize(output_width, offset);
	}
	~GenFlextangle();

	cv::Mat GenerateDevelopedFigure(const std::vector<cv::Mat>& imgs, const std::vector<std::vector<cv::Point2f> >& hexagons) const;
	void Initialize(int output_width, const cv::Point& offset);

private:
	cv::Size _output_size;
	cv::Point _offset;
	int _len_triangle;

	// Bind corner ID between hexagon and development view
	std::vector<cv::Vec3i> hex_corner_ids;
	std::vector<std::vector<cv::Vec3i> > dev_corner_ids;

	// Corner Coordinates of development view
	std::vector<cv::Point2f> dev_corner_coordinates;

	// Initialize Functions
	void BindCorners();
	void GetDevCornerCoordinates();

	// Draw Outline
	void DrawOutline(cv::Mat& draw_img) const;

	// Transfer src_img in src_triangle to dst_img in dst_triangle
	static void TransferTriangle(const cv::Mat& src_img, const std::vector<cv::Point2f>& src_triangle, 
		cv::Mat& dst_img, const std::vector<cv::Point2f>& dst_triangle);

	static float CrossProduct(const cv::Vec2f& vec1, const cv::Vec2f& vec2);

	inline static bool CompareSign(float a, float b)
	{
		return (a > 0 && b > 0) || (a < 0 && b < 0) || (a == 0 && b == 0);
	}

	static bool isInsidePolygon(const std::vector<cv::Point2f>& polygon, const cv::Point2f& pt);


	template <typename T>
	static void dotline(cv::Mat& img, const cv::Point_<T>& pt1, const cv::Point_<T>& pt2, 
		const cv::Scalar& color, int dot_len = 3, int thickness = 1, int lineType = 8, int shift = 0) {

		cv::Point_<T> vec = pt2 - pt1;
		double len = cv::norm(vec);
		vec = vec / len;
		double t = 0;
		while (t < len) {
			cv::Point_<T> bpt = pt1 + vec * t;
			double s = t + dot_len;
			if (s > len) s = len;
			cv::Point_<T> ept = pt1 + vec * s;
			cv::line(img, bpt, ept, color, thickness, lineType, shift);
			t += 2 * dot_len;
		}
	}

};

#endif