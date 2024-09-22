def kmeans(k, points, max_iter=300):

    if k >= len(points):
        exit("Invalid number of clusters!")
    clusters = [[] for _ in range(k)]
    centers = [None for _ in range(k)]
    last_centers = [None for _ in range(k)]
    difference = [None for _ in range(k)]
    iter = 0
    cntr = 0
    for i in range(k):
        centers[i] = points[i]

    while iter < max_iter and cntr < k:
        clusters = [[] for _ in range(k)]
        cntr = 0
        iter += 1
        for point in points:
            clusters[closest_cluster(point, centers)].append(point)

        for i in range(k):
            last_centers[i] = centers[i]
            centers[i] = calc_centroid(clusters[i])
            difference[i] = sub_points(last_centers[i], centers[i])

        for i in range(k):
            cntr += diff_zero(difference[i])
    labels = []
    for point in points:
        for i, cluster in enumerate(clusters):
            if point in cluster:
                labels.append(i)
    return labels


def diff_zero(delta):
    cntr = len(delta)
    for j in delta:
        if j != 0:
            return 0
        else:
            cntr -= 1
    if cntr == 0:
        return 1
    else:
        return 0


def closest_cluster(points, centers):
    min_norma = norma(sub_points(points, centers[0]))
    min_index = 0

    for i, center in enumerate(centers[1:]):
        current_norma = norma(sub_points(center, points))
        if current_norma < min_norma:
            min_norma = current_norma
            min_index = i + 1
    return min_index


def calc_centroid(cluster):
    res = cluster[0]
    for point in cluster[1:]:
        res = add_points(res, point)
    return tuple(coor / len(cluster) for coor in res)


def norma(point):
    return sum([i ** 2 for i in point])


def add_points(first_point, sec_point):
    return tuple(first_point[i] + sec_point[i] for i in range(len(first_point)))


def sub_points(first_point, sec_point):
    return tuple(first_point[i] - sec_point[i] for i in range(len(first_point)))


# if __name__ == '__main__':
#     file_path = sys.argv[3]
#     if not file_path.endswith(".txt"):
#         exit()
#     if (len(sys.argv)) > 1:
#         k = int(sys.argv[1])
#         if k <= 0:
#             exit("Invalid number of clusters!")
#         if len(sys.argv) == 3:
#             max_iter = int(sys.argv[2])
#             if max_iter <= 0 or max_iter >= 1000:
#                 exit("Invalid maximum iteration!")
#             kmeans(k, max_iter)
#         else:
#             kmeans(k)
