#ifndef POLYRASTER_H
#define POLYRASTER_H

#include<vector>
#include<iostream>
#include<fstream>
#include<algorithm>

using namespace std;

struct dvec2 {
	double x;
	double y;
	dvec2() {

	}
	dvec2(double x0, double y0) {
		x = x0;
		y = y0;
	}
};

struct polygon2d {
	int id;
	vector<dvec2> points;
};

bool lineIntersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, float &x, float &y) {
	double det = (y4-y3)*(x2-x1);
	if (det == 0.0) return false;
	double ua = ((x4-x3)*(y1-y3)-(y4-y3)*(x1-x3));
	double ub = ((x2-x1)*(y1-y3));

	if ((det*ub <= 0.0 || det*ub > det*det)) return false;

	x = x1 + ua*(x2 - x1)/det;
	y = y1 + ua*(y2 - y1)/det;
	return true;
}
void loadPolygonsFromASCII(string inputFile, vector<polygon2d> &polygons) {
	ifstream fin(inputFile.c_str());
	polygons.clear();
	while (!fin.eof()) {		
		string line;
		stringstream ss;
		getline(fin,line);
		if (line == "END") break;
		ss << line;
		polygon2d p;
		ss >> p.id;

		getline(fin,line);
		
		while (line != "END") {
			stringstream ss1;
			ss1 << line;
			dvec2 point;
			ss1 >> point.x >> point.y;
			p.points.push_back(point);
			getline(fin,line);
		}
		polygons.push_back(p);		
	}
}
void polyToRaster(const vector<dvec2> points, ximage &img, float cellSize, float &xCorner, float &yCorner, bool overlay = false) {	
	vec2 minCorner = vec2( 9999999999.9, 9999999999.9);
	vec2 maxCorner = vec2(-9999999999.9,-9999999999.9);
	for (int i = 0; i < points.size(); i++) {
		if (points[i].x < minCorner.x) {
			minCorner.x = points[i].x;
		} 
		if (points[i].y < minCorner.y) {
			minCorner.y = points[i].y;
		}
		if (points[i].x > maxCorner.x) {
			maxCorner.x = points[i].x;
		} 
		if (points[i].y > maxCorner.y) {
			maxCorner.y = points[i].y;
		}
	}
	vec2 size = maxCorner - minCorner;
	size.x = max(size.x,size.y);
	double offset = size.x*0.25;
	size.x += offset;

	size.y = size.x;
	if (!overlay) {
		xCorner = minCorner.x - offset*0.5;
		yCorner = minCorner.y - offset*0.5;
		img = ximage((int)size.x/cellSize,(int)size.y/cellSize,1,XIMAGE_FORMAT_RGB24);
		img.fill(vec4(0,0,0,0));
	} else {
		if (minCorner.x < xCorner || maxCorner.x > xCorner + (maxCorner - minCorner).x
			|| minCorner.y < yCorner || maxCorner.y > yCorner + (maxCorner - minCorner).y) {
				//cout << "ERROR: Polygon out of bounds" << endl;
		}
	}

	for (int i = 0; i < img.height(); i++) {
		dvec2 line_p1(xCorner,i*cellSize + yCorner);
		dvec2 line_p2(xCorner + size.x, i*cellSize + yCorner);

		vector<int> intersectPoints;
		for (int j = 0; j < points.size(); j++) {
			dvec2 line_p3 = points[j];
			dvec2 line_p4 = points[(j+1)%points.size()];
			float x,y;
			if (lineIntersect(line_p2.x,line_p2.y,line_p1.x,line_p1.y,line_p3.x,line_p3.y,line_p4.x,line_p4.y,x,y)) {
					x -= xCorner;
					x /= cellSize;
					intersectPoints.push_back(x);
			}
		}

		if (intersectPoints.empty()) continue;
		//cout << intersectPoints.size() << endl;
		sort(intersectPoints.begin(),intersectPoints.end());
		if (intersectPoints.size()%2) {
			//intersectPoints.erase(unique(intersectPoints.begin(), intersectPoints.end()), intersectPoints.end());
			//cout << "WTF!?" << endl;
		}
	
		for (int j = 0; j < intersectPoints.size()-1; j+=2) {
			for (int k = intersectPoints[j]; k < intersectPoints[j+1]; k++) {
				img(k,i)[0] = 255;
				img(k,i)[1] = 255;
				img(k,i)[2] = 255;
			}
		}
	}
}




#endif