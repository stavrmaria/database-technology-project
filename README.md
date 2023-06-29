# Database Technology Project
## Introduction
In this project we deal with the organization and processing of spatial data (spatial data). Spatial data in their simplest form are points in a multidimensional space, such as for example points in the plane or in three-dimensional space. However, we can easily create points in spaces with more dimensions. In practice, the multitude of dimensions is a parameter that is taken into account when organizing the data and when processing queries.

## Structure
The implemented structure is a R*-tree and is an improvement of the R-tree. The data is from OpenStreetMap which allows us to download regions of the world with points of interest.  
The records are saved in a file (datafile.dat) which will consist of blocks of size B = 32KB. Each block contains a set of records and also has a unique  blockid.  
The directory (index) is also stored in a separate file and it organizes the records stored in the datafile. Therefore, in the catalog leaves the coordinates of the point are stored with a Record ID that essentially points to which block of the datafile, the specific record is stored and in which slot.  
The R*-tree supports the following functions:
 - Insertion
 - Deletion
 - Range query
 - k-nn query
 - Bottom-up tree construction

## Note
- Each block has a size of 1KB = 1024 bytes
- main.cpp creates 2 files indexfile and datafile

## Todo List
- [X] Insert
- [X] Change entries (save only the coordinates)
- [ ] Delete
- [ ] Bulk loading
- [x] Support range queries
- [x] Support k-nn queries
- [x] Support skyline queries

### Test file visualization (14 points)
![Test file visualization](/extra/visualization.png)