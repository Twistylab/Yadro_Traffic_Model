#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <filesystem>

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
		void generate_csv(std::filesystem::path path) {
			std::ofstream output_file(path / "result/result_csv.csv");

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
		float avg_package_size;
		float avg_interval;
		std::mt19937 gen;
	public:
		PoissonTrafficModel(float dur, int avgps, float avgt) {
			std::random_device rd;
			gen = std::mt19937(rd());

			duration = dur;
			avg_interval = avgps;
			avg_package_size = avgt;
		};
		void generate_csv(std::filesystem::path path) {
			std::ofstream output_file(path / "result/result_csv.csv");

			output_file << "time,size\n";

			std::exponential_distribution<float> exp_dist(1.0 / avg_package_size);

			float current_time = 0.0;

			while (current_time <= duration) {
				std::poisson_distribution<int> poisson_dist((duration-current_time) / avg_interval);
				int package_number = poisson_dist(gen);
				if (package_number == 0.0) {
					output_file << current_time << "," << static_cast<int>(exp_dist(gen));
					break;
				}
				float interval = (duration-current_time) / package_number;
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

	std::string file_path = argv[1];

	std::filesystem::path path(argv[1]);

	std::filesystem::path path_to_save = path.parent_path().parent_path();

	std::ifstream input_file(file_path);

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
		std::ofstream output_file(path_to_save / "output.csv");
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

		EqualTrafficModel(duration, package_size, interval).generate_csv(path_to_save);
	}
	else if (model == "poisson") {
		float avg_interval;
		float avg_package_size;

		params_stream >> avg_package_size >> avg_interval;

		PoissonTrafficModel(duration, avg_package_size, avg_interval).generate_csv(path_to_save);
	}
	else {
		std::cout << "Unknown model name: " << model << std::endl;
		return 1;
	}

	return 0;
}
