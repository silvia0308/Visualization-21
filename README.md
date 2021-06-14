# Temporal Visualization from Cosmology Simulation
Project for Visualization 2021 Spring @ ETH Zurich [[Report Link](https://drive.google.com/file/d/1uH99tzCAtBD_VBpx9SppVzqJJK4DLgXU/view?usp=sharing)] <br/>
<br/>
This repository contains the source code to support temproal visualization taking cosmology simulation data as inputs.
- Particle type
- Generate data
- Particle attributes
- Cosmic web
- GUI
- Animation

Assume the source code is ran in Linux with vtk, eigen3, qt and opencv installed.
Please run CMake to set corresponding library paths and create project files in release mode, compile code with ```make```.

## Particle type

- Visualize particle types with point Gaussian <br/>
Run ```./particle_type/visual_type/show_type```, follow the instructions to input cosmology path and time.

- Visual the number of particle type transition in a chord diagram <br/>
Open the terminal, run the following commands in the directory ```Visualization-21```:<br/>
```cp runDataDist.sh build/particle_type/charts/runDataTrans.sh```<br/> 
```cd build/particle_type/charts```<br/>
```./runDataTrans.sh /path/of/cosmology/folder```<br/>
And you will get ```Trans.txt``` in ```build/particle_type/```.<br/>
Get into [Colaboratory](https://colab.research.google.com/), upload ```particle_type/charts/chordDiagram.ipynb``` and ```Trans.txt```, run the code.<br/>
You will get the following image:<br/>
<div align=center><img src="https://github.com/silvia0308/Visualization-21/blob/main/particle_type/charts/chordDiagram.png" width="250" height="225"/></div>

- Visual the number of particle types over time in a histogram <br/>
Open the terminal, run the following commands in ```Visualization-21```:<br/>
```cp runDataDist.sh build/particle_type/charts/runDataDist.sh```<br/> 
```cd build/particle_type/charts```<br/>
```./runDataDist.sh /path/of/cosmology/folder```<br/>
And you will get ```distribute.txt``` in ```build/particle_type/```.<br/>
Then run ```./particle_type/charts/plotDist```.<br/>
You will get the following image:<br/>
<div align=center><img src="https://github.com/silvia0308/Visualization-21/blob/main/particle_type/charts/histogram.png" width="250" height="200"/></div>


## Generate data

- Generate txt files of SPH and cosmic web<br/>
Run ```./generate_data/src/sphTest```, follow the instructions to input cosmology path.<br/>
Run ```./generate_data/src/cosmicwebTest```, follow the instructions to input cosmology path.<br/>
You can find the generated files in ```build/generate_data/```.

- Sample data<br/>
Sample SPH result files of different attributes at selected time steps can be found in [sph_results](https://drive.google.com/drive/folders/1KgbL8lGfb9D3dBPeup1ge6lJksZEuOoA?usp=sharing).<br/> 
Sample cosmic web files are in [cosmic_web_results](https://drive.google.com/drive/folders/1rfVVeBU-5SO9LlArOsag71d0c97fO5oX?usp=sharing).<br/> 
You can use them to try out the following visualization methods.

## Particle attributes

- Visualize SPH results with direct volume rendering <br/>
Run ```./particle_attributes/SimpleRayCast```, follow the instructions to input sph result path, attribute and time.  

## Cosmic web

- Visualize cosmic web with point Gaussian <br/>
Run ```./cosmic_web/show_cosmicweb```, follow the instructions to input cosmic web result path and time. 

## GUI

- Integrate all visualization to GUI<br/>
Run ```./gui/MainWindow```, follow the instructions to input cosmology path, SPH and cosmic web result path (in one folder) and time.<br/>
The gui works as the same as in [demo.mov](https://drive.google.com/file/d/1pIoIlcz4T5n609MJ3BvA-8TqXHxdj9Y9/view?usp=sharing).

## Animation

- Show how particles change over time<br/>
[particle_type.avi](https://drive.google.com/file/d/1mIia0titN9ZWJEEr8O1zSwVomJ-G76eR/view?usp=sharing) shows the change of particle types.<br/>
[density.avi](https://drive.google.com/file/d/1S_T4Oa8VnCMWTR_2dIIJCh2WFBA9P1EW/view?usp=sharing) shows the change of particle density.<br/>
If you want to visual the evolution of different particle attributes, run ```./animation/animation```, follow the instructions to input sph result path and attribute.<br/>
You can find the generated video in ```build/animation/```.
