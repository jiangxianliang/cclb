import re
# define k here
k = 4
num_of_links_in_tree = k * k * k


def main():
    """
    """
    lineCounter = 0
    containedLinks = []
    link_counter = 0

    file_read = open('mapping.txt', 'r')
    line = file_read.readline()

    while line != '':
        strArr = re.split(' ', line)
        for link in strArr:
            if ':' not in link:
                containedLinks.append(link)
                link_counter += 1
        if (lineCounter % k) == 0:
            if (link_counter == num_of_links_in_tree):
                print "error"
            link_counter = 0
        lineCounter += 1

if __name__ == '__main__':
    main()
