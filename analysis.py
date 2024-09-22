import sys
import numpy as np
from numpy import random
import pandas as pd
from sklearn.metrics import silhouette_score
import kmeans
import mysymnmf
import math
np.random.seed(0)


def analysis(k: int, data :list):
    # find the symnmf using symnmf.py the same way
    n=len(data)
    norm_matrix = mysymnmf.norm(data)
    if k <= 0:
            raise ValueError("k must be greater than zero.")
    try:
        h = np.random.uniform(0, math.sqrt(2*(np.average(norm_matrix)/k)), (n, k))
        h = h.tolist()
        h = mysymnmf.symnmf(h,norm_matrix)
    except Exception as e:
        print(f"exception in symnmf: {str(e)}")
        raise e
    # getting the symnmf clustering and silhouette score of them
    symnmf_labels = [np.argmax(np.array(row), axis=0) for row in h]
    symnmf_silhouette = silhouette_score(X=data, labels=symnmf_labels)

    # getting the kmeans clustering and silhouette score of them
    kmeans_labels = kmeans.kmeans(k, data)
    kmeans_silhouette = silhouette_score(X=data, labels=kmeans_labels)

    # Print the silhouette scores
    
    if symnmf_silhouette is not None:
        print(f"nmf: {symnmf_silhouette:.4f}")
    if kmeans_silhouette is not None:
        print(f"kmeans: {kmeans_silhouette:.4f}")




if __name__== "__main__":
    file_name = sys.argv[2]
    try:
        k= int(sys.argv[1])
        data = np.genfromtxt(file_name, dtype=float, encoding=None, delimiter=",")
        points=data.tolist()
    except FileNotFoundError:
        exit("file not found.")
    except pd.errors.EmptyDataError:
        exit("file without data.")
    except pd.errors.ParserError:
        exit("file parse error.")
    except Exception as e:
        print(f"unexpected error occurred: {str(e)}")
        exit("could not resolve analisys.")
        
    if len(sys.argv) != 3:
        exit("Wrong parameters!")
    analysis(k, points)
    
