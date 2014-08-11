var tree1 = {
    name:'Root',
    status: 'normal',
    description: 'This is the root N',
    children:[
        {
            name: 'N1',
            status: 'normal',
            description: 'This is the N 1',
            children: [
            ]
        },
        {
            name: 'N2',
            status: 'normal',
            description: 'This is the N 2',
            children: []
        }
    ]
}

var tree2 = {
    name:'Root',
    status: 'normal',
    description: 'This is the root N',
    children:[
        {
            name: 'N1',
            status: 'normal',
            description: 'This is the N 1',
            children: [
                {
                    name: 'N3',
                    status: 'normal',
                    description: 'This is the N 3',
                    children: []
                },
                {
                    name: 'N11',
                    status: 'normal',
                    description: 'This is the N 11',
                    children: []
                }
            ]
        },
        {
            name: 'N2',
            status: 'normal',
            description: 'This is the N 2',
            children: [
                {
                    name: 'N4',
                    status: 'normal',
                    description: 'This is the N 4',
                    children: []
                },
                {
                    name: 'N5',
                    status: 'normal',
                    description: 'This is the N 5',
                    children: []
                },
                {
                    name: 'N6',
                    status: 'normal',
                    description: 'This is the N 6',
                    children: []
                }
            ]
        },
        {
            name: 'N10',
            status: 'normal',
            description: 'This is the N 10',
            children: [
                {
                    name: 'N7',
                    status: 'normal',
                    description: 'This is the N 7',
                    children: []
                },
                {
                    name: 'N8',
                    status: 'normal',
                    description: 'This is the N 8',
                    children: []
                }
            ]
        },
    ]
}

var tree3 = {
    name:'Root',
    children:[
        {
            name: 'N1',
            children: [
                {
                    name: 'N3',
                    children: [
                        {
                            name: 'N6',
                            children: []
                        },
                        {
                            name: 'N7',
                            children: []
                        }
                    ]
                }
            ]
        },
        {
            name: 'N2',
            children: [
                {
                    name: 'N4',
                    children: []
                },
                {
                    name: 'N5',
                    children: [
                        {
                            name: 'N9',
                            children: [
                                {
                                    name: 'N8',
                                    children: []
                                }
                            ]
                        }
                    ]
                }
            ]
        }
    ]
}
