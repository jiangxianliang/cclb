eSDN: Per rack share
================================================

This respository contains the ns-2.35 code for endhost data center controllers.

It is built by extending ns-2.35 by adding support for endhost controllers.

Each endhost at the data center runs an openflow style controller that reads
network statistics from the switches by polling them at short (RTT) time
scales.

For overhead reduction, there are schemes for statistics sharing.

Currently, there are:
 - per-rack sharing
 - per-pod sharing
 - per-pod bottom up

Master branch has the code for per-rack sharing and there is a seperate branch
for each of the other sharing schemes.

INSTALLATION
============

To run this code, clone this repository into your /ns-allinone-2.35/ns-2.35/
directory and replace all the files in there by the files in this repository.

Then use FatGraphGenerator.py to generate a FatTree with k value of your
choice. This will produce files out.txt, mapping.txt and out.tcl.
The format for mapping .txt has *nodeID :: list of links which this node is responsible for
polling*

- Place out.txt, and mapping.txt in the /ns-allinone-2.35/ns-2.35/common/
  directory. These files contain topology information and mappings for info
  sharing DHT and are read from node.cc file.

Run out.tcl from ns to get the experiment going.
