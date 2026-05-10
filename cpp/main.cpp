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
		float delay;
	public:
		EqualTrafficModel(float dur, int ps, float dl) {
			duration = dur;
			package_size = ps;
			delay = dl;
		};
		void generate_csv(std::filesystem::path path) {
			std::ofstream output_file(path / "result/result_csv.csv");

			output_file << "time,size\n";

			float current_time = 0.0;

			while (current_time <= duration) {
				output_file << current_time << "," << package_size << "\n";
				current_time += delay;
			}

			output_file.close();
	}
};

class PoissonTrafficModel {
	private:
		float duration;
		float avg_package_size;
		float avg_delay;
		std::mt19937 gen;
	public:
		PoissonTrafficModel(float dur, float avgps, float avgd) {
			std::random_device rd;
			gen = std::mt19937(rd());

			duration = dur;
			avg_delay = avgd;
			avg_package_size = avgps;
		};
		void generate_csv(std::filesystem::path path) {
			std::ofstream output_file(path / "result/result_csv.csv");

			output_file << "time,size\n";

			std::exponential_distribution<float> size_dist(1.0 / avg_package_size);
			std::exponential_distribution<float> delay_dist(1.0 / avg_delay);

			float current_time = 0.0;

			while (current_time <= duration) {
				output_file << current_time << "," << (int)size_dist(gen) + 1 << "\n";
				current_time += delay_dist(gen);
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
		float delay;

		params_stream >> package_size >> delay;

		if (delay <= 0.0) {
			std::cout << "Delay must be more than 0.0" << std::endl;
			return 1;
		}
		else if (package_size <= 0) {
			std::cout << "Package size must be more than 0" << std::endl;
			return 1;
		}

		EqualTrafficModel(duration, package_size, delay).generate_csv(path_to_save);
	}
	else if (model == "poisson") {
		float avg_delay;
		float avg_package_size;

		params_stream >> avg_package_size >> avg_delay;

		if (avg_delay <= 0.0) {
			std::cout << "Average delay must be more than 0.0" << std::endl;
			return 1;
		}
		else if (avg_package_size <= 0.0) {
			std::cout << "Average package size must be more than 0.0" << std::endl;
			return 1;
		}

		PoissonTrafficModel(duration, avg_package_size, avg_delay).generate_csv(path_to_save);
	}
	else {
		std::cout << "Unknown model name: " << model << std::endl;
		return 1;
	}

	return 0;
}
