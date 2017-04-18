/**
* geoprocessing.h
* Header File designed to TODO
*/

#ifndef GEOPROCESSING_H
#define GEOPROCESSING_H

#include<xlib.h>
#include<fstream>
#include<string>

using namespace std;

struct ESRI_GRID {

	int				nrows;
	int				ncols;
	double			xCorner;
	double			yCorner;
	double			cellSize;
	double			nodataVal;
	double			minValue;
	double			maxValue;
	xarray<float>	dem;

	//Method designed to TODO
	void loadFromFile(string filename) {	
		ifstream fin(filename.c_str());
		if (fin.fail()) {
			cout << "Error: Failed to open file: " << filename << endl;
			exit(1);
		}
		
		string trash;
		fin >> trash >> ncols;
		fin >> trash >> nrows;
		fin >> trash >> xCorner;
		fin >> trash >> yCorner;
		fin >> trash >> cellSize;
		fin >> trash >> nodataVal;

		dem = xarray<float>(nrows,ncols);
		minValue = 99999999;
		maxValue = -99999999;
		for (int i = 0; i < nrows; i++) {
			if (!(i%20)) cout << 100*float(i)/(nrows-1) << "%" << endl;
			for (int j = 0; j < ncols; j++) {
				fin >> dem(i,j);
				if (dem(i,j) != nodataVal) {
					if (dem(i,j) > maxValue) maxValue = dem(i,j);
					if (dem(i,j) < minValue) minValue = dem(i,j);
				}
			}
		}
	}

	//Method designed to TODO
	void  exportToFile(string filename) {
		ofstream fout(filename.c_str());
		if (fout.fail()) {
			cout << "Error: Failed to open file: " << filename << endl;
			exit(1);
		}
		
		fout << "ncols         " << ncols << endl;
		fout << "nrows         " << nrows << endl;
		fout << "xllcorner     " << xCorner << endl;
		fout << "yllcorner     " << yCorner << endl;
		fout << "cellsize      " << cellSize << endl;
		fout << "NODATA_value  " << nodataVal << endl;
		int count = 0;
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				fout << dem(i,j) << " ";
				//count++;
				//if (count % 100 == 0) fout << endl;
			}
		}
	}

	//Method designed to TODO
	void fromImage(const ximage &img) {
		dem = xarray<float>(nrows,ncols);
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				dem(i,j) = *(const float*)img(j,i)*(maxValue - minValue) + minValue;
			}
		}
	}

	//Method designed to TODO
	ximage toImage() {
		ximage result(ncols,nrows,1,XIMAGE_FORMAT_GRAYSCALE_FLOAT32);
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				if (dem(i,j) != nodataVal) {
					*(float*)result(j,i) = (dem(i,j) - minValue)/(maxValue - minValue);
				} else {
					*(float*)result(j,i) = 0;
				}
			}
		}
		return result;
	}
};


#endif