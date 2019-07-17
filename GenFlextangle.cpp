#include "GenFlextangle.h"
#include <opencv2/imgcodecs.hpp>


GenFlextangle::GenFlextangle()
{
}


GenFlextangle::~GenFlextangle()
{
}


void GenFlextangle::Initialize(int output_width, const cv::Point& offset)
{
	int w = output_width - 2 * offset.x;
	_len_triangle = (int)(2.0 * w / (6.2 * std::sqrt(3.0)) + 0.5);
	_output_size.height = 2.5 * _len_triangle + 2 * offset.y;
	_output_size.width = output_width;
	_offset = offset;

	BindCorners();
	GetDevCornerCoordinates();
}


cv::Mat GenFlextangle::GenerateDevelopedFigure(const std::vector<cv::Mat>& imgs, const std::vector<std::vector<cv::Point2f> >& hexagons) const
{
	assert(imgs.size() == hexagons.size());

	cv::Mat output_img(_output_size, CV_8UC3);
	
	output_img.setTo(cv::Scalar(255, 255, 255));

	for (int i = 0; i < hexagons.size(); i++) {
		for (int j = 0; j < hex_corner_ids.size(); j++) {
			std::vector<cv::Point2f> src_triangle(3), dst_triangle(3);
			for (int k = 0; k < 3; k++) {
				src_triangle[k] = hexagons[i][hex_corner_ids[j][k]];
				dst_triangle[k] = dev_corner_coordinates[dev_corner_ids[i][j][k]];
			}
			TransferTriangle(imgs[i], src_triangle, output_img, dst_triangle);
		}
	}
	DrawOutline(output_img);
	return output_img;
}


void GenFlextangle::BindCorners()
{
	hex_corner_ids.resize(6);
	for (int i = 0; i < 6; i++) {
		hex_corner_ids[i] = cv::Vec3i(i+1, 0, i+2);
	}
	hex_corner_ids[5][2] = 1;

	dev_corner_ids.resize(4);
	for(int i=0;i<4;i++)
		dev_corner_ids[i].resize(6);

	// 1st Image
	dev_corner_ids[0][0] = cv::Vec3i(0, 1, 3);
	dev_corner_ids[0][1] = cv::Vec3i(3, 7, 6);
	dev_corner_ids[0][2] = cv::Vec3i(6, 7, 9);
	dev_corner_ids[0][3] = cv::Vec3i(9, 13, 12);
	dev_corner_ids[0][4] = cv::Vec3i(12, 13, 15);
	dev_corner_ids[0][5] = cv::Vec3i(15, 19, 18);

	// 2nd Image
	dev_corner_ids[1][0] = cv::Vec3i(3, 4, 7);
	dev_corner_ids[1][1] = cv::Vec3i(7, 10, 9);
	dev_corner_ids[1][2] = cv::Vec3i(9, 10, 13);
	dev_corner_ids[1][3] = cv::Vec3i(13, 16, 15);
	dev_corner_ids[1][4] = cv::Vec3i(15, 16, 19);
	dev_corner_ids[1][5] = cv::Vec3i(1, 4, 3);

	// 3rd & 4th Image
	dev_corner_ids[2] = dev_corner_ids[0];
	dev_corner_ids[3] = dev_corner_ids[1];
	for (int i = 2; i < 4; i++) {
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < 3; k++) {
				dev_corner_ids[i][j][k] += 1;
			}
		}
	}
}


void GenFlextangle::GetDevCornerCoordinates()
{
	float h_len_traiangle = (float)_len_triangle / 2;
	float var_len = std::sqrt(3.0) * h_len_traiangle;

	dev_corner_coordinates.resize(21);
	for (int i = 0; i < 7; i++) {
		int k = 3 * i;
		dev_corner_coordinates[k] = _offset;
		dev_corner_coordinates[k].x += var_len * i;
		if (i % 2 > 0) {
			dev_corner_coordinates[k].y += h_len_traiangle;
		}
		for (int j = 1; j < 3; j++) {
			dev_corner_coordinates[k + j] = dev_corner_coordinates[k + j - 1];
			dev_corner_coordinates[k + j].y += _len_triangle;
		}
	}
}


// Draw Outline
void GenFlextangle::DrawOutline(cv::Mat& draw_img) const
{
	cv::line(draw_img, dev_corner_coordinates[0], dev_corner_coordinates[18], cv::Scalar(0, 0, 0));
	dotline(draw_img, dev_corner_coordinates[18], dev_corner_coordinates[20], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[20], dev_corner_coordinates[17], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[17], dev_corner_coordinates[14], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[14], dev_corner_coordinates[11], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[11], dev_corner_coordinates[8], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[8], dev_corner_coordinates[5], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[5], dev_corner_coordinates[2], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[2], dev_corner_coordinates[0], cv::Scalar(0, 0, 0));

	cv::line(draw_img, dev_corner_coordinates[0], dev_corner_coordinates[3], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[3], dev_corner_coordinates[6], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[6], dev_corner_coordinates[9], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[9], dev_corner_coordinates[12], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[12], dev_corner_coordinates[15], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[15], dev_corner_coordinates[18], cv::Scalar(0, 0, 0));

	float tablen = 0.2 * _len_triangle;
	cv::Point2f endtab1 = dev_corner_coordinates[18];
	endtab1.x += tablen;
	endtab1.y += tablen;
	cv::Point2f endtab2 = dev_corner_coordinates[19];
	endtab2.x += tablen;
	endtab2.y -= tablen;
	cv::Point2f endtab3 = dev_corner_coordinates[19];
	endtab3.x += tablen;
	endtab3.y += tablen;
	cv::Point2f endtab4 = dev_corner_coordinates[20];
	endtab4.x += tablen;
	endtab4.y -= tablen;
	cv::line(draw_img, dev_corner_coordinates[18], endtab1, cv::Scalar(0, 0, 0));
	cv::line(draw_img, endtab1, endtab2, cv::Scalar(0, 0, 0));
	cv::line(draw_img, endtab2, dev_corner_coordinates[19], cv::Scalar(0, 0, 0));
	cv::line(draw_img, dev_corner_coordinates[19], endtab3, cv::Scalar(0, 0, 0));
	cv::line(draw_img, endtab3, endtab4, cv::Scalar(0, 0, 0));
	cv::line(draw_img, endtab4, dev_corner_coordinates[20], cv::Scalar(0, 0, 0));

	cv::Point2f gluep1(dev_corner_coordinates[3].x, dev_corner_coordinates[0].y);
	cv::Point2f gluep2(dev_corner_coordinates[9].x, dev_corner_coordinates[0].y);
	cv::Point2f gluep3(dev_corner_coordinates[15].x, dev_corner_coordinates[0].y);
	dotline(draw_img, gluep1, dev_corner_coordinates[3], cv::Scalar(0, 0, 0));
	dotline(draw_img, gluep2, dev_corner_coordinates[9], cv::Scalar(0, 0, 0));
	dotline(draw_img, gluep3, dev_corner_coordinates[15], cv::Scalar(0, 0, 0));

	///// Put Text ////
	std::vector<cv::Point2f> txts;
	cv::Point2f txt_pt;
	float txt_offset_x1 = _len_triangle / 3;
	float txt_offset_x2 = txt_offset_x1 / 3;
	float txt_y = gluep1.y + _len_triangle / 5;
	txt_pt = cv::Point2f(gluep1.x - txt_offset_x1, txt_y);
	cv::putText(draw_img, "GLUE", txt_pt, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0));
	txt_pt = cv::Point2f(gluep1.x + txt_offset_x2, txt_y);
	cv::putText(draw_img, "GLUE", txt_pt, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0));
	txt_pt = cv::Point2f(gluep2.x - txt_offset_x1, txt_y);
	cv::putText(draw_img, "GLUE", txt_pt, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0));
	txt_pt = cv::Point2f(gluep2.x + txt_offset_x2, txt_y);
	cv::putText(draw_img, "GLUE", txt_pt, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0));
	txt_pt = cv::Point2f(gluep3.x - txt_offset_x1, txt_y);
	cv::putText(draw_img, "GLUE", txt_pt, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0));
	txt_pt = cv::Point2f(gluep3.x + txt_offset_x2, txt_y);
	cv::putText(draw_img, "GLUE", txt_pt, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0));

}


void GenFlextangle::TransferTriangle(const cv::Mat& src_img, const std::vector<cv::Point2f>& src_triangle,
	cv::Mat& dst_img, const std::vector<cv::Point2f>& dst_triangle)
{
	// Proc Area 
	cv::Point2f min_pt = dst_triangle[0], max_pt = dst_triangle[0];
	for (int i = 1; i < 3; i++) {
		if (min_pt.x > dst_triangle[i].x)
			min_pt.x = dst_triangle[i].x;
		if (max_pt.x < dst_triangle[i].x)
			max_pt.x = dst_triangle[i].x;
		if (min_pt.y > dst_triangle[i].y)
			min_pt.y= dst_triangle[i].y;
		if (max_pt.y < dst_triangle[i].y)
			max_pt.y = dst_triangle[i].y;
	}
	int bx = (int)(min_pt.x - 0.5);
	int by = (int)(min_pt.y - 0.5);
	int ex = (int)(max_pt.x + 0.5);
	int ey = (int)(max_pt.y + 0.5);

	int w = ex - bx + 1;
	int h = ey - by + 1;

	// Create Map
	cv::Mat A = cv::getAffineTransform(dst_triangle, src_triangle);
	cv::Mat dst_pt(3, h*w, CV_64FC1);
	cv::Mat map_x(h, w, CV_32FC1), map_y(h, w, CV_32FC1);
	int c = 0;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			dst_pt.at<double>(0, c) = bx + x;
			dst_pt.at<double>(1, c) = by + y;
			dst_pt.at<double>(2, c) = 1;
			c++;
		}
	}
	cv::Mat src_pt = A * dst_pt;

	c = 0;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			map_x.at<float>(y, x) = src_pt.at<double>(0, c);
			map_y.at<float>(y, x) = src_pt.at<double>(1, c);
			c++;
		}
	}

	// Transform
	cv::Mat trunc_dst(h, w, CV_8UC3);
	cv::remap(src_img, trunc_dst, map_x, map_y, cv::INTER_LINEAR);
	cv::Mat tmp_img;
	for (int y = by; y <= ey; y++) {
		for (int x = bx; x <= ex; x++) {
			if (isInsidePolygon(dst_triangle, cv::Point(x, y))) {
				dst_img.at<cv::Vec3b>(y, x) = trunc_dst.at<cv::Vec3b>(y-by, x-bx);
			}
		}
	}
}


float GenFlextangle::CrossProduct(const cv::Vec2f& vec1, const cv::Vec2f& vec2)
{
	return vec1[0] * vec2[1] - vec1[1] * vec2[0];
}


bool GenFlextangle::isInsidePolygon(const std::vector<cv::Point2f>& polygon, const cv::Point2f& pt)
{
	cv::Vec2f pprev = polygon.back() - pt;
	cv::Vec2f prev = polygon[0] - pt;
	float prev_cp = CrossProduct(pprev, prev);
	for (int i = 1; i < polygon.size(); i++) {
		cv::Vec2f vec = polygon[i] - pt;
		float cp = CrossProduct(prev, vec);

		if (!CompareSign(prev_cp, cp))
			return false;
		prev = vec;
		prev_cp = cp;
	}
	return true;
}
