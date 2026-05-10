import os
import sys
import json

import matplotlib.pyplot as plt
import pandas as pd


def traffic_model_analyzer(df):
    df["delay"] = df["time"].diff()

    analysis_dict = dict()

    analysis_dict["avg_package_size"] = round(df["size"].mean(), 2)
    analysis_dict["avg_delay"] = round(df["delay"].mean(), 2)
    analysis_dict["bitrate"] = round(df["size"].sum() / (df["time"].max() - df["time"].min()), 2)

    if round(df["delay"].diff().sum()) == 0:
        analysis_dict["model"] = "equal"
    else:
        analysis_dict["model"] = "poisson"

    fig = plt.figure(figsize=(12, 10))

    ax1 = fig.add_subplot(2, 1, 1)

    df['delay'].hist(bins=30)
    ax1.set_title('Гистограмма задержек между пакетами')
    ax1.set_xlabel('Задержка')
    ax1.set_ylabel('Частота')

    ax2 = fig.add_subplot(2, 1, 2)

    df['size'].hist(bins=30)
    ax2.set_title('Гистограмма размеров пакетов')
    ax2.set_xlabel('Размер')
    ax2.set_ylabel('Частота')

    return fig, analysis_dict


if __name__ == "__main__":
    path = sys.argv[1]
    data_frame = pd.read_csv(path)

    result_dir = os.path.dirname(path)

    hist, analysis_result =  traffic_model_analyzer(data_frame)

    hist.savefig(os.path.join(result_dir, "histograms.png"), dpi=300)

    with open(os.path.join(result_dir, "analysis.json"), "w") as file:
        json.dump(analysis_result, file, indent=4)

