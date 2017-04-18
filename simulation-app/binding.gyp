{
    "targets": [
        {
            "target_name": "simulationmanager",
            "sources": [ 
                "libs/simulations/simulationmanager.cpp",         
            ],
            "include_dirs" : [ 
                "<!(node -e \"require('nan')\")",          
                "libs/xlib", 
                "libs/simulations",    
                "libs/simulations/avalanche-simulation",    
                "libs/simulations/avalanche-simulation/particle",    
                "libs/simulations/avalanche-simulation/terrain",    
                "libs/simulations/avalanche-simulation/resources",    
            ],
        }
    ]
}
