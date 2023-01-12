# SR-NIRS
> Started on Jan/2021

SR-NIRS is a project to develop a low cost, portable, Spatially Resolved Near-Infrared Spectroscopy embedded system. Currently, the project is based on ESP-32 microcontroller.

The goal is to be able to monitor Tissue Oxygenation Index (TOI) from the forearm muscle during arm arterial occlusion experiment in a real time application.

The project also includes the development of a *Python GUI* to interact with the system.

*Undergraduate Final Project*



## Python GUI

This system was developed along with a Python GUI to enable wireless (currently Bluetooth) communication and data logging, as well as user monitoring of the incoming NIRS signal.

****

### Setup - Python Virtual Environment  

**This step, even though recommended, is not required. You can skip to Python Requirements sections if you wish.**

First of all, it is recommended to create a Python Virtual Environment. This can be done through python's embedded  (v. 3.4+) module called **venv**:

```
python -m venv [directory]
``` 

Then, the environment should be activated through:  
- ### Windows  
``` 
[directory]\Scripts\activate.bat
```

- ### Linux or MacOS  
```
source [directory]/bin/activate 
``` 

[More information on python virtual environments can be found here.](https://python.land/virtual-environments/virtualenv)

### Setup - Python Requirements

All of the required packages are included on **_requirements.txt_** file, inside **_GUI_** folder.  

These requirements can be installed through python's **pip** module, as it follows:

```
pip install -r requirements.txt
``` 

### Running GUI

To run the system GUI, the **_main.py_** script should be executed:
```
python main.py
``` 

***
## Documentation

The documentation will be in included in this repository as the project progresses.
