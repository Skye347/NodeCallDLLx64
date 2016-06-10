{
    "targets":[
        {
            "target_name":"rundll",
            "sources":["src/main.cc"],
            "conditions":[
                ['OS=="Windows"',{
                    'ldflags':[
                    
                    ],
                }],
            ],
        },
    ],
},
