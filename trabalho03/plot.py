import pandas as pd
from p_tqdm import t_map
import plotly.express as px


def get_df():
    return pd.read_csv('results.csv', index_col=0)


def group_df(df):
    df = df.groupby(['nx', 'threads', 'vectorized']
                    ).agg({"time": ["mean", "std"]})
    return df


def parse_row(row):
    nx = row[0][0]
    threads = row[0][1]
    vectorized = row[0][2]
    time_mean = row[1][0]
    time_std = row[1][1]
    return {
        "nx": nx,
        "threads": threads,
        "vectorized": vectorized,
        "time_mean": time_mean,
        "time_std": time_std,
    }


def plot(df):
    pass


def main():
    df = group_df(get_df())
    rows = t_map(parse_row, list(df.iterrows()))
    df = pd.DataFrame(rows)
    df["color"] = df.apply(lambda x: "nx" + str(x["nx"]) +
                           "_" + ("vec" if x["vectorized"] else "nonvec"), axis=1)
    df = df[df["threads"] != 16]
    df = df[df["vectorized"] == False]
    fig = px.bar(df, x="threads", y="time_mean",
                 error_y="time_std", color="color")
    fig.update_layout(barmode='group')
    fig.show()
    return df


df = main()
