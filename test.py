import matplotlib.pyplot as plt


def main():
    # read from valuecost100.0000000.txt which contains a data point B,Bmin,Bmax per line. plot (B,Bmin) in blue and (B,Bmax) in red
    # the file is in the format:
    # B,Bmin,Bmax
    # read the file
    print("foo")
    with open("optimal-bounds.txt", "r") as f:
        optBounds = f.readlines()
    # split the lines into B,Bmin,Bmax
    data = [line.split(",") for line in optBounds]
    # convert the data to float
    data = [[float(x) for x in line] for line in data]
    # split the data into B,Bmin,Bmax
    B = [line[0] for line in data[:52]]
    Bmin = [line[1] for line in data[:52]]
    Bmax = [line[2] for line in data[:52]]
    # plot the data
    plt.plot(B, Bmin, "b-", label="Bmin")
    plt.plot(B, Bmax, "r-", label="Bmax")


    #do the same for another file, but plot bMin bMax as points
    with open("nstar=5/valuecost1000.000000.txt", "r") as f:
        optBounds = f.readlines()
    # split the lines into B,Bmin,Bmax
    data = [line.split(",") for line in optBounds]
    # convert the data to float
    data = [[float(x) for x in line] for line in data]
    # split the data into B,Bmin,Bmax
    B2 = [line[0] for line in data if line[1] != 0 or line[2] != 0]

    #filter out any bmin/bmax == 0
    Bmin2 = [line[1] for line in data if line[1] != 0 or line[2] != 0]
    Bmax2 = [line[2] for line in data if line[1] != 0 or line[2] != 0]
    # plot the data as points
    plt.plot(B2, Bmin2, "bo", label="Bmin2")
    plt.plot(B2, Bmax2, "ro", label="Bmax2")

    # add labels and title
    plt.xlabel("B")
    plt.ylabel("Bmin/Bmax")
    plt.title("Bmin and Bmax vs B")
    plt.legend()
    # show the plot
    print("foor")
    plt.show()
    print("foo")
    # save the plot
    plt.savefig("valuecost100.0000000.png")
    # close the plot
    plt.close()
    # return the data
  

    return 

main()
