# Temporal Visualization from Cosmology Simulation
Project for Visualization 2021 Spring @ ETH Zurich [[Report Link]()] <br/>
<br/>
This repo contains the source code to support temproal visualization taking cosmology simulation data as inputs.
- Generate data
- Particle type
- Particle attributes
- Cosmic web
- GUI
- Animation

Please build the code properly with cmake and vtk, eigen3 and qt installed.

## Generate Data

- Generate txt files of SPH and cosmic web<br/>
Run ```generate_data/src/main_sph.cpp```, follow the instructions to input cosmology path.<br/>
Run ```generate_data/src/main_cosmicweb.cpp```, follow the instructions to input cosmology path.<br/>
The generated files are saved in the build folder.

## Particle type

- Visualize particle types with point gaussian <br/>
Run ```particle_type/visual_type/show.cxx```, follow the instructions to input cosmology path and time.<br/>
- Visual the number of particle type transition in a chord diagram <br/>
Get into Colaboratory via the link https://colab.research.google.com/. Open ```particle_type/charts/chordDiagram.ipynb```.<br/>
You will get the following image:<br/>
<div align=center><img src="https://github.com/B1ueber2y/Visualization-21/blob/Final/particle_type/charts/chordDiagram.png" width="450" height="400"/></div>

- Visual the number of particle types over time in a histogram <br/>
Run ```particle_type/charts/dataDistribu.cpp```, follow the instructions to input cosmology path and get ```distribute.txt```. Then run ```particle_type/charts/plotDist.cpp```.<br/>
You will get the following image:<br/>
<div align=center><img src="https://github.com/B1ueber2y/Visualization-21/blob/Final/particle_type/charts/histogram.png" width="450" height="400"/></div>

## Particle attributes

- Visualize SPH results with direct volume rendering <br/>
Run ```particle_attributes/SimpleRayCast.cxx```, follow the instructions to input sph result path, attribute and time.  

## Cosmic web

- Visualize cosmic web with point gaussian <br/>
Run ```cosmic_web/show.cxx```, follow the instructions to input cosmic web result path and time. 

## GUI

- Integrate all visualization in a GUI<br/>
Run ```gui/main.cxx```, follow the instructions to input cosmology path, SPH and cosmic web result path (in one folder) and time.<br/>
The gui works as the same as in the ```gui/demo.mov```.

## Animation

- Show how particles change over time<br/>
```animation/particle_type.avi``` shows the change of particle types.<br/>
```animation/density.avi``` shows the change of particle density.<br/>
If you want to visual the change of different particle attributes, run ```animation/SimpleRayCast.cxx```, follow the instructions to input sph result path and attribute.
