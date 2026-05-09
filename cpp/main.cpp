#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

#include <string>

class EqualTrafficModel {
	private:
		float duration;
		int package_size;
		float interval;
	public:
		EqualTrafficModel(float d, int p, float t) {
			duration = d;
			package_size = p;
			interval = t;
		};
		void generate_csv() {
			std::ofstream output_file("output.csv");

			output_file << "time,size\n";

			float current_time = 0.0;

			while (current_time <= duration) {
				output_file << current_time << "," << package_size << "\n";
				current_time += interval;
			}

			output_file.close();
	}
};

class PoissonTrafficModel {
	private:
		float duration;
		int mean_package_size;
		float mean_interval;
		std::mt19937 gen;
	public:
		PoissonTrafficModel(float d, int mp, float mt) {
			std::random_device rd;
			gen = std::mt19937(rd());

			duration = d;
			mean_interval = mt;
			mean_package_size = mp;
		};
		void generate_csv() {
			std::ofstream output_file("output.csv");

			output_file << "time,size\n";

			std::poisson_distribution<int> poisson_dist(duration / mean_interval);

			float interval = duration / poisson_dist(gen);

			std::exponential_distribution<float> exp_dist(1.0 / mean_package_size);

			float current_time = 0.0;

			while (current_time <= duration) {
				output_file << current_time << "," << static_cast<int>(exp_dist(gen)) << "\n";
				current_time += interval;
			}

			output_file.close();
	}
};

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage: ./traffic_model <input_file>" << std::endl;
		return 1;
	}

	std::cout << "Traffic model started" << std::endl;

	std::string path = argv[1];

	std::cout << "Input file: " << path << std::endl;

	std::ifstream input_file(path);

	std::string line1;
	std::getline(input_file, line1);
	std::string line2;
	std::getline(input_file, line2);

	float duration = std::stof(line1);

	if (duration < 0.0 || duration > 3600.0) {
		std::cout << "Duration must be in range [0, 3600]" << std::endl;
		return 1;
	}
	else if (duration == 0.0) {
		std::ofstream output_file("output.csv");
		output_file << "time,size\n0.0,0";
		output_file.close();
		return 0;
	}

	std::size_t pos = line2.find(':');

	std::string model = line2.substr(0, pos);

	std::istringstream params_stream(line2.substr(pos + 1));

	if (model == "equal") {
		int package_size;
		float interval;

		params_stream >> package_size >> interval;

		EqualTrafficModel(duration, package_size, interval).generate_csv();
	}
	else if (model == "poisson") {
		float mean_interval;
		int mean_package_size;

		params_stream >> mean_package_size >> mean_interval;

		PoissonTrafficModel(duration, mean_package_size, mean_interval).generate_csv();
	}
	else {
		std::cout << "Unknown model name: " << model << std::endl;
		return 1;
	}

	return 0;
}
