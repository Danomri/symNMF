from re import fullmatch
import sys
import pandas as pd
import numpy as np
from numpy import random
import math
import mysymnmf
np.random.seed(0)


def symnmff(k : int, goal : str, data : list):
    n=len(data)
    
    if goal == 'sym':
        sym_matrix = mysymnmf.sym(data)
        return sym_matrix
    elif goal == 'ddg':
        ddg_matrix = mysymnmf.ddg(data)
        return ddg_matrix

    elif goal == 'norm' or goal=='symnmf':
        norm_matrix = mysymnmf.norm(data)
        if goal == 'norm':
            return norm_matrix
        elif goal == 'symnmf':
            if k <= 0:
                raise ValueError("k must be greater than zero.")
            m = np.average(norm_matrix)
            if m < 0:
                raise ValueError("Average value of norm_matrix (m) must be non-negative.")
            try:
                #creat random matrix h
                h_bot = [0 for i in range(k)]
                h_top= [(math.sqrt(m / k) * 2) for i in range(k)]
                h = np.random.uniform(low=h_bot, high=h_top, size=(n,k))
                h = h.tolist()
                h = mysymnmf.symnmf(h,norm_matrix)
                return h
            except Exception as e:
                raise e
    else:
        raise ValueError("Unknown goal. Supported goals are 'sym', 'ddg', 'norm', 'symnmf'.")


    
    

if __name__== "__main__":
    if len(sys.argv) != 4:
        exit("Wrong parameters. Usage: <k> <goal> <file_name>")
    
    try:
        file_name = sys.argv[3]
        goal = sys.argv[2]
        k= int(sys.argv[1])
        data = pd.read_csv(file_name, header=None).values.tolist()
        result= symnmff(k, goal, data)
    
        if result is not None:
             for row in result:
                 str_list = ["%.4f" % cell for cell in row]
                 print(*str_list, sep=',')

    except FileNotFoundError:
        exit("file not found.")
    except pd.errors.EmptyDataError:
        exit("file - no data.")
    except pd.errors.ParserError:
        exit("file - parse error.")
    except Exception as e:
        print(f"Unexpected error occurred: {str(e)}")
        exit("any other exception.")
        
        