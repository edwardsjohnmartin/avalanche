#ifndef GENETIC_PROG_H
#define GENETIC_PROG_H
#include "avalanchesim.h"
#include <windows.h>
#include <process.h>
#include <ctime>

const char* Gene_labels[] = {"initialHeight","bounceFriction","stickyness","dampingForce","turbulanceForce","clumpingFactor","viscosity","gridSize"};
const float Gene_minVals[] = {0.0, 0.0, 0.1, 0.0, 0.25, 0.0, 0.0, 64.0};
const float Gene_maxVals[] = {100.0, 0.25, 0.75, 0.15, 5.0, 1.0, 1.0, 512.0};

float g_mutationChance = 0.333;
float g_mutationScale = 0.25;
float g_mseThreshold = 2.0/255.0;

struct Gene {
	double fitness;			//fitness level of this gene
	double sequence[8];     //sequence values (range 0 - 1)
	Gene() {
		fitness = 999;
	}
};

ximage distanceTransform(const ximage& src, float threshold) {
	ximage dist(src.width(),src.height(),1,XIMAGE_FORMAT_GRAYSCALE_FLOAT32);
	dist.fill(vec4(0,0,0,0));
	for (int i = 0; i < dist.width(); i++) {
		//cout << double(i)/dist.width() << endl;
		for (int j = 0; j < dist.height(); j++) {
			float minDist = 999999;
			float ijval = ((unsigned char)src(i,j)[0])/255.0;
			for (int x = 0; x < src.width(); x++) {
				for (int y = 0; y < src.height(); y++) {
					float xyval = ((unsigned char)src(x,y)[0])/255.0;
					if ((xyval > threshold && ijval <= threshold)||
						(xyval < threshold && ijval >= threshold)) {
						float tmpDist = ((i - x)*(i - x) + (j - y)*(j - y));
						if (tmpDist < minDist) {
							minDist = tmpDist;
						}
					}
				}
			}
			*(float*)dist(i,j) = sqrt(2.0*minDist/(src.width()*src.height()));
		}
	}

	return dist;
}

void combine(const Gene &a, const Gene &b, Gene &newA, Gene& newB) {
	//combines 2 genes to form 2 new genes
	//this code does random stuff such as sequence cross-overs, linear combination and random mutation given
	//the 2 parent genese

	//sequence cross over
	/*int pivot = rand()%8;
	for (int i = 0; i < pivot; i++) {
		newA.sequence[i] = a.sequence[i];
		newB.sequence[i] = b.sequence[i];
	}
	for (int i = pivot; i < 8; i++) {
		newA.sequence[i] = b.sequence[i];
		newB.sequence[i] = a.sequence[i];
	}*/


	for (int i = 0; i < 8; i++) {
		if (rand()%2) {
			newA.sequence[i] = a.sequence[i];
			newB.sequence[i] = b.sequence[i];
		} else {
			newA.sequence[i] = b.sequence[i];
			newB.sequence[i] = a.sequence[i];
		}
		if (frand()<g_mutationChance) {
			float tridist = (frand()-frand())*g_mutationScale*frand();
			if (rand()%2) newA.sequence[i] += tridist;
			else newB.sequence[i] += tridist;
		}
	}

	int pivot;
	//random mutation
	float rnd = frand();
	if (rnd < g_mutationChance) {
		pivot = rand()%8;
		if (rand()%2) {
			newA.sequence[pivot] += g_mutationScale*(frand()-frand());
		} else {
			newB.sequence[pivot] += g_mutationScale*(frand()-frand());
		}
	}

	//random linear interpolation of the pivot point
	rnd = frand();
	if (rnd < g_mutationChance) {
		pivot = rand()%8;
		float alpha = frand();
		if (rand()%2) {
			newA.sequence[pivot] = alpha*a.sequence[pivot] + (1.0 - alpha)*b.sequence[pivot];
		} else {
			newB.sequence[pivot] = alpha*a.sequence[pivot] + (1.0 - alpha)*b.sequence[pivot];
		}
	}

	//fix each value in the sequence between 0 and 1
	for (int i = 0; i < 8; i++) {
		newA.sequence[i] = fclamp(newA.sequence[i],0.0,1.0);
		newB.sequence[i] = fclamp(newB.sequence[i],0.0,1.0);
	}
}
float evalFitness(const Gene &a, vector<MassMovementSimulator*> &simulators) {
	//computes the fitness levels of each gene on a given array of simulators
	//returns the fitness level as the Mean Squared Error (lower is more fit)

	double mse = 0;
	double count = 1.0;
	for (int i = 0; i < simulators.size(); i++) {
		//copy parameters from gene
		simulators[i]->args.initialHeight		 = (a.sequence[0])*(Gene_maxVals[0] - Gene_minVals[0]) + Gene_minVals[0];	
		simulators[i]->args.bounceFriction		 = (a.sequence[1])*(Gene_maxVals[1] - Gene_minVals[1]) + Gene_minVals[1];
		simulators[i]->args.stickyness			 = (a.sequence[2])*(Gene_maxVals[2] - Gene_minVals[2]) + Gene_minVals[2];
		simulators[i]->args.dampingForce		 = (a.sequence[3])*(Gene_maxVals[3] - Gene_minVals[3]) + Gene_minVals[3];
		simulators[i]->args.turbulanceForce		 = (a.sequence[4])*(Gene_maxVals[4] - Gene_minVals[4]) + Gene_minVals[4];
		simulators[i]->args.clumpingFactor		 = (a.sequence[5])*(Gene_maxVals[5] - Gene_minVals[5]) + Gene_minVals[5];
		simulators[i]->args.viscosity			 = (a.sequence[6])*(Gene_maxVals[6] - Gene_minVals[6]) + Gene_minVals[6];
		simulators[i]->args.gridSize			 = (a.sequence[7])*(Gene_maxVals[7] - Gene_minVals[7]) + Gene_minVals[7];
		
		//reset the simulation
		simulators[i]->initParticles();

		//run the simulation loop
		simulators[i]->mainLoop();

		
		//compute the mse by comparing the output image (forceMap) with the input training image (pathImage)
		for (float u = 0; u < 1.0; u += 1.0/simulators[i]->pathImage.width()) {
			for (float v = 0; v < 1.0; v += 1.0/simulators[i]->pathImage.height()) {
				float err = simulators[i]->forceMap.getPixelLerp(u,v).x;

				//here we are computing a weighted error. We use the weight from the distance map to scale the total error
				//meaning the farther away a pixel is from the flow path, the the larger error weight balue.
				float weight = simulators[i]->pathDistanceMap.getPixelLerp(u,v).x*0.5 + 0.5;

				//here we cap the error off based on its threshold.
				if (err > g_mseThreshold) err = g_mseThreshold;

				//scale the err val to be within 0-1 for comparing the with the path data
				err *= 1.0 / g_mseThreshold;

				//compute the actual error
				err = err - simulators[i]->pathImage.getPixelLerp(u,v).x;
				
				//here we apply a bias of 1.5 more error if the error falls inside of the path (negative error)
				//this biases the error such that under-fitting the input path is worse than over fitting.
				if (err < 0.0) err *= 1.5;

				//square the value so its always positive (for mean square error calculation
				err = err*err;

				//apply the error weight and add it up to the mse
				mse += err*weight;
				count += weight;
			}
		}
	}
	//mse = sqrt(mse);
	mse /= count;

	return mse;
}

void crossover(vector<Gene> &populationA, vector<Gene> &populationB) {
	//crossover a random individual from 2 different populations
	double af = frand();
	af = af*af;
	int a = clamp(int(af*populationA.size()),0,(int)populationA.size()-1);
	Gene tmp = populationA[a];
	populationA[a] = populationB[a];
	populationB[a] = tmp;
}



void runGA(int numGenerations, vector<Gene> &population, vector<MassMovementSimulator*> &simulators) {
	//run the Genetic Algorithm on a given an initial population and a number of generation



	//compute initial finitness levels
	/*for (int i = 0; i < population.size(); i++) {
		if (population[i].fitness >= 998) population[i].fitness = evalFitness(population[i],simulators);
	}*/

	//sort population
	for (int j = 0; j < population.size(); j++) {
		for (int k = j+1; k < population.size(); k++) {
			if (population[j].fitness > population[k].fitness) {
				Gene tmp = population[j];
				population[j] = population[k];
				population[k] = tmp;
			}
		}
	}


	/*cout << "Fitness Levels: " << endl;
	for (int j = 0; j < population.size(); j++) {
		cout << population[j].fitness << endl;
	}
	cout << endl;
	*/
	


	for (int i = 0; i < numGenerations; i++) {
		
		//cout << "*** Generation " << i << " ***" << endl;

		//pick the 2 fit for reproduction
		//uses a polynomial (x^3) curve to bias towards picking more higher fit sequences for reproduction 
		float af;
		int a;
		for (int j = 0; j < 1; j++) {
			af = frand();
			af = af*af*af;				
			a = clamp(int(af*population.size()),0,(int)population.size()-1);
			int b = a;
			while (b == a) {
				float bf = frand();
				bf = bf*bf*bf;
				b = clamp(int(bf*population.size()),0,(int)population.size()-1);
			}
			Gene newA;
			Gene newB;
			combine(population[a],population[b],newA,newB);
			//compute fitness of new genes:
			newA.fitness = evalFitness(newA,simulators);
			newB.fitness = evalFitness(newB,simulators);
			population.push_back(newA);
			population.push_back(newB);
		}


		//sort population
		for (int j = 0; j < population.size(); j++) {
			for (int k = j+1; k < population.size(); k++) {
				if (population[j].fitness > population[k].fitness) {
					Gene tmp = population[j];
					population[j] = population[k];
					population[k] = tmp;
				}
			}
		}


		//Prune off 4 unfit sequences
		//uses a polynomial (1.0 - x^3) curve to bias towards picking least fit sequences for pruning
		for (int j = 0; j < 2; j++) {
			population.pop_back();
			//af = frand();
			//af = 1.0 - af*af*af;
			//a = clamp(int(af*population.size()),0,(int)population.size()-1);
			//population.erase(population.begin()+a);
		}

		/*cout << "Fitness Levels: " << endl;
		for (int j = 0; j < population.size(); j++) {
			cout << population[j].fitness << endl;
		}
		cout << endl;
		cout << "***" << endl;
		cout << Gene_labels[0] << " " << (population[0].sequence[0])*(Gene_maxVals[0] - Gene_minVals[0]) + Gene_minVals[0] << endl;
		cout << Gene_labels[1] << " " << (population[0].sequence[1])*(Gene_maxVals[1] - Gene_minVals[1]) + Gene_minVals[1] << endl;
		cout << Gene_labels[2] << " " << (population[0].sequence[2])*(Gene_maxVals[2] - Gene_minVals[2]) + Gene_minVals[2] << endl;
		cout << Gene_labels[3] << " " << (population[0].sequence[3])*(Gene_maxVals[3] - Gene_minVals[3]) + Gene_minVals[3] << endl;
		cout << Gene_labels[4] << " " << (population[0].sequence[4])*(Gene_maxVals[4] - Gene_minVals[4]) + Gene_minVals[4] << endl;
		cout << Gene_labels[5] << " " << (population[0].sequence[5])*(Gene_maxVals[5] - Gene_minVals[5]) + Gene_minVals[5] << endl;
		cout << Gene_labels[6] << " " << (population[0].sequence[6])*(Gene_maxVals[6] - Gene_minVals[6]) + Gene_minVals[6] << endl;
		cout << Gene_labels[7] << " " << (population[0].sequence[7])*(Gene_maxVals[7] - Gene_minVals[7]) + Gene_minVals[7] << endl;
		cout << "***" << endl;

		
		//output 
		simulators[0]->args.initialHeight		 = (population[0].sequence[0])*(Gene_maxVals[0] - Gene_minVals[0]) + Gene_minVals[0];	
		simulators[0]->args.bounceFrictionForce  = (population[0].sequence[1])*(Gene_maxVals[1] - Gene_minVals[1]) + Gene_minVals[1];
		simulators[0]->args.stickynessFactor	 = (population[0].sequence[2])*(Gene_maxVals[2] - Gene_minVals[2]) + Gene_minVals[2];
		simulators[0]->args.dampingForce		 = (population[0].sequence[3])*(Gene_maxVals[3] - Gene_minVals[3]) + Gene_minVals[3];
		simulators[0]->args.turbulanceForce		 = (population[0].sequence[4])*(Gene_maxVals[4] - Gene_minVals[4]) + Gene_minVals[4];
		simulators[0]->args.clumpingFactor		 = (population[0].sequence[5])*(Gene_maxVals[5] - Gene_minVals[5]) + Gene_minVals[5];
		simulators[0]->args.viscosity			 = (population[0].sequence[6])*(Gene_maxVals[6] - Gene_minVals[6]) + Gene_minVals[6];
		simulators[0]->args.gridSize			 = (population[0].sequence[7])*(Gene_maxVals[7] - Gene_minVals[7]) + Gene_minVals[7];
		simulators[0]->initParticles();
		simulators[0]->mainLoop();
		simulators[0]->forceMap.exportAs_BMP("evolution2_" + toString(i) + ".bmp");*/
		
	
	}
}

#define MAX_NUM_THREADS 64
volatile bool THREAD_DONE[MAX_NUM_THREADS];


struct ThreadData {
	vector<Gene>*	population;
	vector<MassMovementSimulator*>* simulators;
	int				threadIndex;

};

void GAThread(void* arg) {
	ThreadData* tdata = (ThreadData*)arg;
	int tIndex = tdata->threadIndex;
	runGA(10,*(tdata->population), *(tdata->simulators));
	THREAD_DONE[tIndex] = true;
}

struct GASettings {
	string			logFile;
	int				numPopulations;
	int				populationSize;
	int				numGenerations;
	float			mutationScale;
	float			mutationChance;
	float			mutationScaleDamping;
	float			mseThreshold;
	vector<int>		trainingSets;
	vector<int>		verificationSets;
	GASettings() {
		logFile =				"galog.txt";
		numPopulations	=		1;
		populationSize	=		10;
		numGenerations	=		100;
		mutationScale	=		0.25;
		mutationChance	=		0.25;
		mutationScaleDamping =	0.05;
	}
	void parseSettingsFile(const string& settingsFile) {
		ifstream fin(settingsFile.c_str());
		if (fin.fail()) {
			cout << "ERROR: Failed to open file " << settingsFile << endl;
		}


		while (!fin.eof()) {
			string tmpline;
			stringstream line;
			getline(fin,tmpline);
			line << tmpline;
			string param;
			line >> param;
			if (param.empty()) continue;
			if (param[0] == '#') continue;
			if (param == "logFile") {
				line >> logFile;
			} else if (param == "numPopulations") {
				line >> numPopulations;
			} else if (param == "populationSize") {
				line >> populationSize;
			} else if (param == "numGenerations") {
				line >> numGenerations;
			} else if (param == "mutationScale") {
				line >> mutationScale;
			} else if (param == "mutationChance") {
				line >> mutationChance;
			} else if (param == "mutationScaleDamping") {
				line >> mutationScaleDamping;
			} else if (param == "mseThreshold") {
				line >> mseThreshold;
			} else if (param == "trainingSets") {
				while (!line.eof()) {
					int tmp;
					line >> tmp;
					trainingSets.push_back(tmp);
				}
			} else if (param == "verificationSets") {
				while (!line.eof()) {
					int tmp;
					line >> tmp;
					verificationSets.push_back(tmp);
				}
			} 
		}

	}
};

void batchGA(string gaSettingsFile, vector<Gene> &bestFitPopulation) {

	GASettings settings;
	settings.parseSettingsFile(gaSettingsFile);
	ofstream fout(settings.logFile.c_str());
	vector<Gene> population[MAX_NUM_THREADS];
	vector<MassMovementSimulator*> simulators[MAX_NUM_THREADS];
	bestFitPopulation.clear();
	g_mutationScale = settings.mutationScale;
	g_mseThreshold = settings.mseThreshold;
	g_mutationChance = settings.mutationChance;
	
	//load up training data

	cout << "Loading Training Datasets...";
	for (int t = 0; t < settings.numPopulations; t++) {
		for (int i = 0; i < settings.trainingSets.size(); i++) {
			SimulationSettings args;
			args.parseSettings("simulationsettings.txt");
			args.elevationDEMFile =		"training_data/data_"+toString(settings.trainingSets[i])+"_dem.txt";
			args.startingZoneFile =		"training_data/data_"+toString(settings.trainingSets[i])+"_startzones.bmp";
			args.pathFile =				"training_data/data_"+toString(settings.trainingSets[i])+"_path.bmp";
			args.pathDistanceMap =		"training_data/data_"+toString(settings.trainingSets[i])+"_pathdistance.bmp";
			//args.maxIterations = 2000;
			args.flowPathOutputFile = "";
			args.verboseOutput = false;

			MassMovementSimulator* sim = new MassMovementSimulator();
			sim->args = args;
			sim->pathDistanceMap.importFrom_BMP("training_data/data_"+toString(settings.trainingSets[i])+"_pathdistance.bmp");

			sim->init(false);
			simulators[t].push_back(sim);
			cout << ".";
		}
		cout << ".";
	}
	cout << "done." << endl;

	cout << "Building Initial Populations...";
	for (int t = 0; t < settings.numPopulations; t++) {
		for (int i = 0; i < settings.populationSize; i++) {
			Gene a;
			a.sequence[0] = (frand()*Gene_maxVals[0] - Gene_minVals[0])/(Gene_maxVals[0] - Gene_minVals[0]);
			a.sequence[1] = (frand()*Gene_maxVals[1] - Gene_minVals[1])/(Gene_maxVals[1] - Gene_minVals[1]);
			a.sequence[2] = (frand()*Gene_maxVals[2] - Gene_minVals[2])/(Gene_maxVals[2] - Gene_minVals[2]);
			a.sequence[3] = (frand()*Gene_maxVals[3] - Gene_minVals[3])/(Gene_maxVals[3] - Gene_minVals[3]);
			a.sequence[4] = (frand()*Gene_maxVals[4] - Gene_minVals[4])/(Gene_maxVals[4] - Gene_minVals[4]);
			a.sequence[5] = (frand()*Gene_maxVals[5] - Gene_minVals[5])/(Gene_maxVals[5] - Gene_minVals[5]);
			a.sequence[6] = (frand()*Gene_maxVals[6] - Gene_minVals[6])/(Gene_maxVals[6] - Gene_minVals[6]);
			a.sequence[7] = (frand()*Gene_maxVals[7] - Gene_minVals[7])/(Gene_maxVals[7] - Gene_minVals[7]);
			population[t].push_back(a);
			cout << ".";
		}
		/*cout << "Initial Population " << t  << ": " << endl;
		for (int i = 0; i < population[t].size(); i++) {
			cout << "*** Gene " << i << " ***" << endl;
			for (int j = 0; j < 8; j++) {
				cout << Gene_labels[j] << " " << (population[t][i].sequence[j])*(Gene_maxVals[j] - Gene_minVals[j]) + Gene_minVals[j] << endl;
			}
			cout << endl;
		}
		cout << endl;
		*/
		cout << ".";
	}
	cout << "done." << endl;


	cout << "Starting Genetic Optimization..." << endl;
	cout << "Logging genetic data to galog.txt" << endl;
	for (int i = 0;  i < settings.numGenerations; i++) {
		ThreadData tdata[MAX_NUM_THREADS];
		fout << "Generation: " << i << endl;
		cout << "Generation: " << i << endl;


		for (int t = 1; t < settings.numPopulations; t++) {
			THREAD_DONE[t] = false;
			tdata[t].threadIndex = t;
			tdata[t].simulators = &simulators[t];
			tdata[t].population = &population[t];
			_beginthread(GAThread,0,&tdata[t]);
		}

		THREAD_DONE[0] = false;
		tdata[0].threadIndex = 0;
		tdata[0].simulators = &simulators[0];
		tdata[0].population = &population[0];
		GAThread(&tdata[0]);
		


		for (;;) {
			static int ticker = 0;
			bool done = true;
			Sleep(32);
			for (int t = 1; t < settings.numPopulations; t++) {
				if (!THREAD_DONE[t]) done = false;
			}
			if (done) break;
			ticker++;
			if (!(ticker%100)) {
				cout << ".";
			}
		}


		cout <<  endl;
		for (int t = 0; t < settings.numPopulations; t++) {
			fout << "Population " << t << endl;
			for (int k = 0; k < population[t].size(); k++) {
				fout << "Gene " << k << endl;
				fout << "Fitness " << population[t][k].fitness << endl;
				for (int j = 0; j < 8; j++) {
					fout << Gene_labels[j] << "\t\t" << (population[t][k].sequence[j])*(Gene_maxVals[j] - Gene_minVals[j]) + Gene_minVals[j] << endl;
				}
				fout << endl;
			}
			fout << endl;
			
		}
		fout << endl;
		//find best:
		Gene best = population[0][0];
		
		for (int j = 1; j < settings.numPopulations; j++) {
			if (population[j][0].fitness <  best.fitness) {
				best = population[j][0];
			}
		}
		if (bestFitPopulation.empty()) {
			for (int j = 0; j < settings.numPopulations; j++) {
				bestFitPopulation.push_back(population[j][0]);
			}
		} else {
			for (int j = 0; j < settings.numPopulations; j++) {
				if (population[j][0].fitness < bestFitPopulation[j].fitness) {
					bestFitPopulation[j] = population[j][0];
				}
			}
		}

		cout << "Best Fitness:\t\t" << best.fitness << endl;
		fout << "Best Fitness:\t\t" << best.fitness << endl;
		for (int j = 0; j < 8; j++) {
			cout << Gene_labels[j] << "\t\t" << (best.sequence[j])*(Gene_maxVals[j] - Gene_minVals[j]) + Gene_minVals[j] << endl;
			fout << Gene_labels[j] << "\t\t" << (best.sequence[j])*(Gene_maxVals[j] - Gene_minVals[j]) + Gene_minVals[j] << endl;
		}
		cout << endl;
		fout << endl;

		for (int j = 0; j < simulators[0].size(); j++) {
			simulators[0][j]->args.initialHeight		= (best.sequence[0])*(Gene_maxVals[0] - Gene_minVals[0]) + Gene_minVals[0];	
			simulators[0][j]->args.bounceFriction		= (best.sequence[1])*(Gene_maxVals[1] - Gene_minVals[1]) + Gene_minVals[1];
			simulators[0][j]->args.stickyness			= (best.sequence[2])*(Gene_maxVals[2] - Gene_minVals[2]) + Gene_minVals[2];
			simulators[0][j]->args.dampingForce			= (best.sequence[3])*(Gene_maxVals[3] - Gene_minVals[3]) + Gene_minVals[3];
			simulators[0][j]->args.turbulanceForce		= (best.sequence[4])*(Gene_maxVals[4] - Gene_minVals[4]) + Gene_minVals[4];
			simulators[0][j]->args.clumpingFactor		= (best.sequence[5])*(Gene_maxVals[5] - Gene_minVals[5]) + Gene_minVals[5];
			simulators[0][j]->args.viscosity			= (best.sequence[6])*(Gene_maxVals[6] - Gene_minVals[6]) + Gene_minVals[6];
			simulators[0][j]->args.gridSize				= (best.sequence[7])*(Gene_maxVals[7] - Gene_minVals[7]) + Gene_minVals[7];
			simulators[0][j]->initParticles();
			simulators[0][j]->mainLoop();
			for (int r = 0; r < simulators[0][j]->forceMap.height(); r++) {
				for (int c = 0; c < simulators[0][j]->forceMap.width(); c++) {
					if (*(float*)simulators[0][j]->forceMap(c,r) > g_mseThreshold) {
						*(float*)simulators[0][j]->forceMap(c,r) = 1.0;
					} else {
						*(float*)simulators[0][j]->forceMap(c,r) = 0.0;
					}
				}
			}
			ximage difmap = ((simulators[0][j]->pathImage)*0.5 + 0.5) - (simulators[0][j]->forceMap*0.333);
			difmap.exportAs_BMP("output/evolution_iter" + toString(i) + "_dataset" + toString(j) + ".bmp");
		}
		


		for (int j = 0; j < settings.numPopulations; j++) {
			for (int k = j+1; k < settings.numPopulations; k++) {
				if (frand() < 0.01) crossover(population[j],population[k]);
			}
		}
		g_mutationScale *= 0.95;
	}
	
	


	//now that the GA has finished, run the verification
	



	//clean up
	for (int t = 0; t < settings.numPopulations; t++) {
		for (int i = 0; i < settings.trainingSets.size(); i++) {
			delete simulators[t][i];
		}
	}

}





#endif //GENETIC_PROG_H