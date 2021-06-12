# Temporal Visualization from Cosmology Simulation
Project for Visualization 2021 Spring @ ETH Zurich [[Report Link]()] <br/>
<br/>
This repo contains the source code to support temproal visualization taking cosmology simulation data as inputs.
- Generate data
- Particle type
- Particle attributes
- Cosmic web
- GUI

Please build the code properly with cmake and vtk, eigen3 and qt installed.

## Generate Data

- Before visualization, you need to generate txt files of SPH and cosmic web.<br/>
Run ```generate_data/src/main_sph.cpp```, follow the instructions to input cosmology path.<br/>
Run ```generate_data/src/main_cosmicweb.cpp```, follow the instructions to input cosmology path.<br/>
The generated files are saved in the build folder.

## Particle type

- Visualize particle types with point gaussian <br/>
Run ```particle_type/visual_type/show.cxx```, follow the instructions to input cosmology path and time.<br/>
- Visual the number of particle type transition in a chord diagram <br/>
Get into Colaboratory via the link https://colab.research.google.com/. Open ```particle_type/charts/chordDiagram.ipynb```.<br/>
- Visual the number of particle types over time in a histogram <br/>
Run ```particle_type/charts/dataDistribu.cpp```, follow the instructions to input cosmology path and get ```distribute.txt```. Then run ```particle_type/charts/plotDist.cpp```.<br/>

## Particle attributes

- Visualize SPH results with direct volume rendering <br/>
Run ```particle_attributes/SimpleRayCast.cxx```, follow the instructions to input sph result path, attribute and time.  

## Cosmic web


- Visualize cosmic web with point gaussian <br/>
Run ```cosmic_web/show.cxx```, follow the instructions to input cosmic web result path and time. 

## GUI

Run ```gui/main.cxx```, follow the instructions to input cosmology path, SPH and cosmic web result path (in one folder) and time.
