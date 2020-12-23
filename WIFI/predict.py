# from sklearn.svm import SVC
# from micromlgen import port
# import featuretools as ft
# import pandas as pd
# from sklearn import neighbors
# import m2cgen as m2c
# # myroom = pd.read_csv('data/myroom.csv')
# # dadroom = pd.read_csv('data/dadroom.csv')
# # Kitchen = pd.read_csv('data/Kitchen.csv')
# # hall = pd.read_csv('data/hall.csv')
# # passage = pd.read_csv('data/passage.csv')

# # myroom['target']=1
# # dadroom['target']=2
# # Kitchen['target']=3
# # hall['target']=4
# # passage['target']=5
# # features= myroom.append([dadroom,Kitchen,hall,passage],ignore_index=True)
# # classmap=features 
# # put your samples in the dataset folder
# # one class per file
# # one feature vector per line, in CSV format
# features, classmap = ft.load_features('data/')
# X, y = features.loc[:, features.columns != 'target'], features.iloc[:,-1:]

# print(X.shape)
# print("---------------------")
# print(y.shape)
# print(features)
# classifier=neighbors.KNeighborsClassifier()
# y = y.to_numpy()
# y = y.ravel()
# from sklearn.ensemble import RandomForestClassifier
# # classifier=tree.DecisionTreeClassifier()
# # classifier.fit(X,y)
# classifier = RandomForestClassifier(random_state=0)
# classifier.fit(X, y)
# # classifier = SVC(kernel='linear').fit(X, y)
# # classifier.fit(X,y)
# c_code = port(classifier, classmap=classmap)
# print(c_code)

from sklearn.svm import SVC
from micromlgen import port
from glob import glob
from os.path import basename
import numpy as np
from sklearn.ensemble import RandomForestClassifier

def load_features(folder):
    dataset = None
    classmap = {}
    for class_idx, filename in enumerate(glob('%s/*.csv' % folder)):
        class_name = basename(filename)[:-4]
        classmap[class_idx] = class_name
        samples = np.loadtxt(filename, dtype=float, delimiter=',')
        labels = np.ones((len(samples), 1)) * class_idx
        samples = np.hstack((samples, labels))
        dataset = samples if dataset is None else np.vstack((dataset, samples))

    return dataset, classmap
# put your samples in the dataset folder
# one class per file
# one feature vector per line, in CSV format
features, classmap = load_features('data/')
X, y = features[:, :-1], features[:, -1]
classifier = RandomForestClassifier(random_state=0,max_depth=20).fit(X, y)
c_code = port(classifier, classmap=classmap)
print(c_code)