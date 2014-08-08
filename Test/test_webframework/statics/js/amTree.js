function Queue(){
    var buffer = new Array();
    Queue.prototype.push = function(ele){
        if (arguments.length == 0)
            return - 1;
        for (var i = 0; i < arguments.length; ++i){
            buffer.push(arguments[i]);
        }
        return buffer.length;
    }

    Queue.prototype.pop = function(){
        return buffer.length == 0 ? null : buffer.shift();
    }

    Queue.prototype.isEmpty = function(){
        return (buffer.length == 0);
    }
}

CanvasRenderingContext2D.prototype.roundRect = function (x, y, w, h, rad, fill, stroke) {
    if (typeof stroke == "undefined")
        stroke = false;
    if (typeof fill == "undefined")
        fill = true;

    var size = w > h ? h : w;
    if( rad > size * 0.5 )
        rad = size * 0.5;

    this.beginPath();
    this.moveTo(x + rad, y);
    this.lineTo(x + w - rad, y);
    this.quadraticCurveTo(x + w, y, x + w, y + rad);
    this.lineTo(x + w, y + h - rad);
    this.quadraticCurveTo(x + w, y + h, x + w - rad, y + h);
    this.lineTo(x + rad, y + h);
    this.quadraticCurveTo(x, y + h, x, y + h - rad);
    this.lineTo(x, y + rad);
    this.quadraticCurveTo(x, y, x + rad, y);
    this.closePath();
    if (stroke)
        this.stroke();
    if (fill)
        this.fill();
}

function amTree(canvas){
    var canvas = document.getElementById(canvas);
    var context = canvas.getContext("2d");
    var dragIndex = -1;
    var originPosX = 20;
    var nodeWidth = 60;
    var nodeHeight = 34;
    var nodeRadius = 6;
    var originPosY = 20;
    var originParentIndex = -1;
    var crossOverIndex = -1;
    var nodesInfo = [];
    var nodesPath = [];
    var linesInfo = [];
    var dragNodeLinkLines = [];
    var initialized = false;
    var treeToDraw = null;
    var evtHandlers = {};

    this.setTree = function(tree){
        treeToDraw = tree;
        initNodes();
        paint();
    }

    // nodeClicked, treeChanged
    this.addEventHandler = function (evt,handler){
        evtHandlers[evt] = handler;
    }

    canvas.onclick = function(evt){
        var index = findNodeByPos(evt.layerX,evt.layerY);
        if(index < 0 || !evtHandlers.nodeClicked)
            return;
        evtHandlers.nodeClicked(findNodeByIndex(rIndex(index)));
    }

    canvas.onmousedown = function(evt){
        dragIndex = findNodeByPos(evt.layerX,evt.layerY);
        if ( dragIndex == nodesInfo.length - 1 )
            dragIndex = -1;
        if( dragIndex == -1 )
            return;
        originPosX = nodesInfo[dragIndex].x;
        originPosY = nodesInfo[dragIndex].y;
        originParentIndex = findNodeByPos(
            linesInfo[dragIndex].from.x,
            linesInfo[dragIndex].from.y);

        dragNodeLinkLines = [];
        for( var i = 0 ; i < linesInfo.length; ++i){
            if(linesInfo[i].from.node == nodesInfo[dragIndex].name){
                dragNodeLinkLines.push(i);
            }
        }

        console.log(dragNodeLinkLines);
        evt.target.style.cursor = 'move';
    }

    canvas.onmousemove = function(evt){
        if( dragIndex == -1 )
            return;
        var halfWidth = nodeWidth * 0.5;
        var halfHeight = nodeHeight * 0.5;
        var x = evt.layerX <= halfWidth
            ? halfWidth
            : ( evt.layerX <= canvas.width - halfWidth ? evt.layerX : canvas.width - halfWidth );
        var y = evt.layerY <= halfHeight
            ? halfHeight
            : ( evt.layerY <= canvas.height - halfHeight ? evt.layerY : canvas.height - halfHeight );
        nodesInfo[dragIndex].x = x;
        nodesInfo[dragIndex].y = y;
        linesInfo[dragIndex].to.x = x;
        linesInfo[dragIndex].to.y = y;

        for( var i = 0 ; i < dragNodeLinkLines.length; ++i){
            linesInfo[dragNodeLinkLines[i]].from.x = x;
            linesInfo[dragNodeLinkLines[i]].from.y = y;
        }

        crossOverIndex = dragCrossIndex(x,y);
        paint();
    }

    canvas.onmouseout = function(evt){
        if (dragIndex == -1)
            return;
        evt.target.style.cursor = 'auto';
        dragFinish();
    }

    canvas.onmouseup = function(evt){
        if (dragIndex == -1)
            return;
        evt.target.style.cursor = 'auto';
        dragFinish();
    }

    function dragFinish(){
        if (crossOverIndex == -1 || crossOverIndex == originParentIndex) {
            nodesInfo[dragIndex].x = originPosX;
            nodesInfo[dragIndex].y = originPosY;
            linesInfo[dragIndex].to.x = originPosX;
            linesInfo[dragIndex].to.y = originPosY;
            for( var i = 0 ; i < dragNodeLinkLines.length; ++i){
                linesInfo[dragNodeLinkLines[i]].from.x = nodesInfo[dragIndex].x;
                linesInfo[dragNodeLinkLines[i]].from.y = nodesInfo[dragIndex].y;
            }
            dragIndex = -1;
            dragNodeLinkLines = [];
            paint();
            return;
        }

        var dragNode = treeToDraw;
        var dragPath = findPathByIndex(rIndex(dragIndex));
        for(var i = 0; i < dragPath.length; ++i){
            dragNode = dragNode.children[dragPath[i]];
        }

        var destParent = findNodeByIndex(rIndex(crossOverIndex));
        var originParent = findNodeByIndex(rIndex(originParentIndex));

        destParent.children.push(dragNode);
        originParent.children.splice(dragPath[dragPath.length -1],1);

        dragIndex = -1;

        if ( evtHandlers.treeChanged )
            evtHandlers.treeChanged(treeToDraw);

        initNodes();
        paint();
    }

    function rIndex(index){
        return nodesInfo.length - index - 1;
    }

    function findNodeByPos(x,y){
        var info;
        var dist;
        for (var i = 0; i < nodesInfo.length; ++i) {
            info = nodesInfo[i];
            dist = (x - info.x) * (x - info.x) + (y - info.y) * (y - info.y);
            if (pointInRect(x,y,info.x,info.y))
                return i;
        }
        return -1;
    }

    function findPathByIndex(index){
        if( index >= nodesPath.length )
            return -1;
        return nodesPath[index];
    }

    function findNodeByIndex(index){
        if ( index >= nodesInfo.length )
            return null;
        var path = findPathByIndex(index);
        var node = treeToDraw;
        for(var i = 0; i < path.length; ++i){
            node = node.children[path[i]];
        }
        return node;
    }

    function dragCrossIndex(x,y){
        var info;
        var dist;
        for (var i = 0; i < nodesInfo.length; ++i) {
            if ( i == dragIndex)
                continue;
            info = nodesInfo[i];
            dist = (x - info.x) * (x - info.x) + (y - info.y) * (y - info.y);
            if (pointInRect(x,y,info.x,info.y))
                return i;
        }
        return -1;
    }

    function pointInRect(px, py, rx, ry){
        var res = Math.abs(px - rx) <= nodeWidth
            && Math.abs(py - ry) <= nodeHeight;
        return res;
    }

    function treeLayout(tree,level) {
        if (!treeLayout.info) {
            treeLayout.info = [];
        }

        if( level >= treeLayout.info.length ) {
            treeLayout.info.push({count:0,groups:[]});
        }

        treeLayout.info[level].count += 1;
        treeLayout.info[level].groups.push(tree.children.length);
        for(var i = 0; i < tree.children.length; ++i){
            treeLayout(tree.children[i],level + 1);
        }
    }

    function reset(){
        initialized = false;
        dragIndex = -1;
        originPosX = 20;
        originPosY = 20;
        originParentIndex = -1;
        crossOverIndex = -1;
        nodesInfo = [];
        linesInfo = [];
        nodesPath = [];
        dragNodeLinkLines = [];
        treeLayout.info = [];
    }

    function initPaths(){
        var paths = [];
        var queue = new Queue();
        queue.push({node:treeToDraw,path:[]});
        while( !queue.isEmpty() ){
            var val = queue.pop();
            var node = val.node;
            var path = val.path;
            nodesPath.push(path);
            for(var i = 0; i < node.children.length; ++i){
                var cpath = path.slice(0);
                cpath.push(i);
                queue.push({node:node.children[i],path:cpath});
            }
        }
        return paths;
    }

    function initStatus(){
        for(var i = 0; i < nodesInfo.length; ++i) {
            var indx = rIndex(i);
            var node = findNodeByIndex(indx);
            nodesInfo[i].name = node.name;
            nodesInfo[i].id = indx;
        }
    }

    function initPosition(){
        treeLayout(treeToDraw,0);
        var treeInfo = treeLayout.info;
        var yspace = 100;
        var xspace = 120;
        var rad = 80;
        var ny = 20;
        var nx = (canvas.width - rad)* 0.5;

        nodesInfo.push({x:nx,y:ny});
        var infoLen = treeInfo.length;

        for(var i = 0; i < infoLen - 1; ++i){
            ny += yspace;
            var group = treeInfo[i].groups;
            var width = (treeInfo[i + 1].count)* xspace  + rad * treeInfo[i + 1].count;
            var nx = (canvas.width -  width) * 0.5 + rad - nodeWidth * 0.5;
            for(var j = 0; j < group.length; ++j){
                var count = group[j];
                for(var k = 0; k < count; ++k){
                    nodesInfo.push({x:nx,y:ny});
                    nx += rad;
                }
                nx += xspace;
            }
        }

        nodesInfo.reverse();
        var yoffset = (canvas.height - nodesInfo[0].y) * 0.5 - nodeHeight * 0.5;
        for(var i = 0; i < nodesInfo.length; ++i){
            nodesInfo[i].y += yoffset;
        }

        if(infoLen < 2)
            return;

        // bottom-top
        var eindx = 0;
        var sindx = 0;
        var index = 0;
        for(var i = infoLen - 1; i >= 0; --i){
            var group = treeInfo[i].groups;
            for(var j = group.length - 1; j >= 0; --j){
                if (group[j] > 0) {
                    eindx = sindx + group[j] - 1;
                    nodesInfo[index].x = (nodesInfo[sindx].x + nodesInfo[eindx].x) * 0.5;
                }
                sindx += group[j];
                index += 1;
            }
        }

        // top-bottom
        nodesInfo.reverse();
        sindx = 1;
        eindx = treeInfo[1].count;
        var min = 100000;
        var max = 0;
        var diff0 = canvas.width / 2 - nodesInfo[0].x;
        nodesInfo[0].x += diff0;
        for(var i = sindx; i <= eindx; ++i){
            if (nodesInfo[i].x < min)
                min = nodesInfo[i].x;
            else if( nodesInfo[i].x > max)
                max = nodesInfo[i].x;
        }
        var width = (treeInfo[1].count - 1)*(rad + xspace);
        var npos = (canvas.width - width) * 0.5;
        var pos = [];
        for(var i = 0; i < treeInfo[1].count; ++i){
            pos.push(npos - nodesInfo[sindx + i].x);
            nodesInfo[sindx + i].x = npos;
            npos += rad + xspace;
        }
        for(var i = eindx + 1; i < nodesPath.length; ++i){
            nodesInfo[i].x += pos[nodesPath[i][0]];
        }

        nodesInfo.reverse();
    }

    function initLines() {
        var treeInfo = treeLayout.info;
        var infoLen = treeInfo.length;
        var eindx = 0;
        var sindx = 0;
        var index = 0;
        for(var i = infoLen - 1; i >= 0; --i){
            var group = treeInfo[i].groups;
            for(var j = group.length - 1; j >= 0; --j){
                if (group[j] > 0) {
                    eindx = sindx + group[j] - 1;
                    for(var k = 0; k < group[j]; ++k){
                        linesInfo.push({
                            from:{
                                node:nodesInfo[index].name,
                                x:nodesInfo[index].x,
                                y:nodesInfo[index].y
                            },
                            to:{
                                node:nodesInfo[sindx+k].name,
                                x:nodesInfo[sindx+k].x,
                                y:nodesInfo[sindx+k].y
                            }
                        });
                    }
                }
                sindx += group[j];
                index += 1;
            }
        }
    }

    function initNodes() {
        reset();
        initPaths();
        initPosition();
        initStatus();
        initLines();
        initialized = true;
    }

    function paint(){
        if( !initialized )
            return;
        context.clearRect(0,0,canvas.width,canvas.height);
        drawLines();
        drawNodes();
    }

    function drawLines(){
        if ( linesInfo.length == 0 )
            return;

        context.shadowBlur = 1;
        context.shadowColor = '#222';

        for (var i = 0; i < linesInfo.length; ++i){
            var line = linesInfo[i];
            var normal = dragIndex != i && dragNodeLinkLines.indexOf(i) == -1;
            var hl = (line.to.y - line.from.y) * 0.5;

            context.strokeStyle = normal ? '#333' : 'red';

            context.beginPath();
            context.moveTo(line.from.x,line.from.y);
            if( normal ){
                context.lineTo(line.from.x,line.from.y + hl);
                context.lineTo(line.to.x,line.to.y - hl);
            }
            context.lineTo(line.to.x,line.to.y);
            context.stroke();
        }
    }

    function drawNodes(){
        if( nodesInfo.length == 0 )
            return;
        var info;
        var grad;
        context.shadowBlur = 4;
        context.shadowColor = '#222';
        context.textAlign = 'center';
        context.textBaseline = 'middle';
        context.font = "bold 11pt Arial";
        var halfWidth = nodeWidth * 0.5;
        var halfHeight = nodeHeight * 0.5;
        for (var i = 0; i < nodesInfo.length; ++i) {
            info = nodesInfo[i];

            grad = context.createLinearGradient(info.x, info.y - halfHeight, info.x, info.y + halfHeight);
            grad.addColorStop(0,"#0f0");
            grad.addColorStop(1,"#070");

            context.shadowBlur = 4;
            context.fillStyle = grad;
            context.beginPath();
            context.roundRect(
                    info.x - halfWidth,
                    info.y - halfHeight,
                nodeWidth,
                nodeHeight,
                nodeRadius,
                true,
                false);
            context.fill();

            context.shadowBlur = 0;
            context.fillStyle = 'black';
            context.fillText(info.name+'',info.x,info.y)
        }
    }

    this.initNodes = initNodes;
    this.repaint = paint;
}
