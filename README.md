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

Please build the code properly with cmake and vtk, eigen3, qt and opencv installed.

## Generate Data

- Generate txt files of SPH and cosmic web<br/>
Run ```generate_data/src/main_sph.cpp``` (project sphTest), follow the instructions to input cosmology path.<br/>
Run ```generate_data/src/main_cosmicweb.cpp``` (project cosmicwebTest), follow the instructions to input cosmology path.<br/>
You can find the generated files in ```build/generate_data/```.

## Particle type

- Visualize particle types with point gaussian <br/>
Run ```particle_type/visual_type/show.cxx``` (project show_type), follow the instructions to input cosmology path and time.<br/>
- Visual the number of particle type transition in a chord diagram <br/>
Get into Colaboratory via the link https://colab.research.google.com/. Open ```particle_type/charts/chordDiagram.ipynb```.<br/>
You will get the following image:<br/>
<div align=center><img src="https://github.com/B1ueber2y/Visualization-21/blob/Final/particle_type/charts/chordDiagram.png" width="450" height="400"/></div>

- Visual the number of particle types over time in a histogram <br/>
Run ```particle_type/charts/dataDistribu.cpp``` (project dataDistribu), follow the instructions to input cosmology path and get ```distribute.txt```. Then run ```particle_type/charts/plotDist.cpp``` (project plotDist).<br/>
You will get the following image:<br/>
<div align=center><img src="https://github.com/B1ueber2y/Visualization-21/blob/Final/particle_type/charts/histogram.png" width="450" height="400"/></div>

## Particle attributes

- Visualize SPH results with direct volume rendering <br/>
Run ```particle_attributes/SimpleRayCast.cxx``` (project SimpleRayCast), follow the instructions to input sph result path, attribute and time.  

## Cosmic web

- Visualize cosmic web with point gaussian <br/>
Run ```cosmic_web/show.cxx``` (project show_cosmicweb), follow the instructions to input cosmic web result path and time. 

## GUI

- Integrate all visualization in a GUI<br/>
Run ```gui/main.cxx``` (project MainWindow), follow the instructions to input cosmology path, SPH and cosmic web result path (in one folder) and time.<br/>
The gui works as the same as in the [demo.mov](https://drive.google.com/file/d/1pIoIlcz4T5n609MJ3BvA-8TqXHxdj9Y9/view?usp=sharing).

## Animation

- Show how particles change over time<br/>
[particle_type.avi](https://drive.google.com/file/d/1mIia0titN9ZWJEEr8O1zSwVomJ-G76eR/view?usp=sharing) shows the change of particle types.<br/>
[density.avi](https://drive.google.com/file/d/1S_T4Oa8VnCMWTR_2dIIJCh2WFBA9P1EW/view?usp=sharing) shows the change of particle density.<br/>
If you want to visual the change of different particle attributes, run ```animation/SimpleRayCast.cxx``` (project animation), follow the instructions to input sph result path and attribute.<br/>
You can find the generated video in ```build/animation/```.
