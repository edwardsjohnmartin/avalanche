//TODO determine if this file is really needed. There are no references to anything inside of it currently

#ifndef MARCHINGSQUARES_H
#define MARCHINGSQUARES_H

#include<vector>
#include"xlib.h"

using namespace std;

struct Polygon2d {
	vector<vec2> points;
	void addOffset(vec2 offset) {
		for (int i = 0; i < points.size(); i++) {
			points[i] += offset;
		}
	}
};

//TODO: determine if this is needed, as it is not used anywhere
void reducePolygons(vector<Polygon2d> &polygons) {
	int numReduced = 0;
	for(;;) {
		
		for (int i = 0; i < polygons.size(); i++) {
			bool end = false;
			for (int j = 0; j < polygons.size(); j++) {
				if (i != j) {
					if (polygons[i].points.empty()) continue;
					if (polygons[j].points.empty()) continue;
					if ((polygons[i].points.back() - polygons[j].points.front()).lengthSqr() < 0.00001) {
						for (int k = 1; k < polygons[j].points.size(); k++) {
							polygons[i].points.push_back(polygons[j].points[k]);
						}
						numReduced++;
						polygons[j].points.clear();
						end = true;
						break;
					}
				}
			}
			if (end) break;
		}

		if (numReduced == 0) break;
		numReduced = 0;
	}
	vector<Polygon2d> reduced;
	for (int i = 0; i < polygons.size(); i++) {
		if (!polygons[i].points.empty()) {
			reduced.push_back(polygons[i]);
		}
	}
	polygons = reduced;
}

//TODO: determine if this is needed, as it is not used anywhere
void marchingSquares(ximage img, float threshold, float accuracy, vector<Polygon2d> &polygons) {
	img = img.resizedTo(img.width()*accuracy,img.height()*accuracy);
	polygons.clear();
	vector<Polygon2d> cases[16];
	cases[0];

	cases[1].push_back(Polygon2d());
	cases[1][0].points.push_back(vec2(0.5,0));
	cases[1][0].points.push_back(vec2(0,0.5));

	cases[2].push_back(Polygon2d());
	cases[2][0].points.push_back(vec2(1,0.5));
	cases[2][0].points.push_back(vec2(0.5,0));

	cases[3].push_back(Polygon2d());
	cases[3][0].points.push_back(vec2(0,0.5));
	cases[3][0].points.push_back(vec2(1,0.5));

	cases[4].push_back(Polygon2d());
	cases[4][0].points.push_back(vec2(0.5,1));
	cases[4][0].points.push_back(vec2(1,0.5));
	
	cases[5].push_back(Polygon2d());
	cases[5].push_back(Polygon2d());
	cases[5][0].points.push_back(vec2(0.5,1));
	cases[5][0].points.push_back(vec2(0,0.5));
	cases[5][1].points.push_back(vec2(0.5,0));
	cases[5][1].points.push_back(vec2(1,0.5));

	cases[6].push_back(Polygon2d());
	cases[6][0].points.push_back(vec2(0.5,1));
	cases[6][0].points.push_back(vec2(0.5,0));

	cases[7].push_back(Polygon2d());
	cases[7][0].points.push_back(vec2(0.5,1));
	cases[7][0].points.push_back(vec2(0,0.5));

	cases[8].push_back(Polygon2d());
	cases[8][0].points.push_back(vec2(0,0.5));
	cases[8][0].points.push_back(vec2(0.5,1));
	
	cases[9].push_back(Polygon2d());
	cases[9][0].points.push_back(vec2(0.5,0));
	cases[9][0].points.push_back(vec2(0.5,1));
	
	cases[10].push_back(Polygon2d());
	cases[10].push_back(Polygon2d());
	cases[10][0].points.push_back(vec2(1,0.5));
	cases[10][0].points.push_back(vec2(0.5,1));
	cases[10][1].points.push_back(vec2(0,0.5));
	cases[10][1].points.push_back(vec2(0.5,0));

	cases[11].push_back(Polygon2d());
	cases[11][0].points.push_back(vec2(1,0.5));
	cases[11][0].points.push_back(vec2(0.5,1));
	

	cases[12].push_back(Polygon2d());
	cases[12][0].points.push_back(vec2(1,0.5));
	cases[12][0].points.push_back(vec2(0,0.5));

	cases[13].push_back(Polygon2d());
	cases[13][0].points.push_back(vec2(0.5,0));
	cases[13][0].points.push_back(vec2(1,0.5));

	cases[14].push_back(Polygon2d());
	cases[14][0].points.push_back(vec2(0,0.5));
	cases[14][0].points.push_back(vec2(0.5,0));

	cases[15];
	//int accuracyStep = accuracy;
	for (int i = 0; i < (img.width()-1); i++) {
		for (int j = 0; j < (img.height()-1); j++) {
			float pixelVal[4] = {
				*(float*)img(i,j),
				*(float*)img(i+1,j),
				*(float*)img(i+1,j+1),
				*(float*)img(i,j+1)
			};
		
			int b[4] = {
				pixelVal[0] > threshold,
				pixelVal[1] > threshold,
				pixelVal[2] > threshold,
				pixelVal[3] > threshold
			};
			int n = b[0] + b[1]*2 + b[2]*4 + b[3]*8;
			for (int p = 0; p < cases[n].size(); p++) {
				polygons.push_back(cases[n][p]);
				polygons.back().addOffset(vec2(i,j));
			}
		}
	}

	//reducePolygons(polygons);
	
}

#endif