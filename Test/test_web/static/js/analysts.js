var layout = [];
var count = 0;
var dragInfo = {};
var currCtrl = null;
var mainViewBaseHeight;
//window.onresize = resize();

function resize() {
    var mainView = document.getElementById('mainView');
    mainView.width = window.innerWidth - 80;
    mainView.height = window.innerHeight - 40;
    mainView.style.width =  mainView.width + 'px';
    mainView.style.height = mainView.height + 'px';
    mainViewBaseHeight = mainView.height;
}

function allowDrop(evt) {
    evt.preventDefault();
}

function dragAdd(evt) {
    evt.preventDefault();
    dragInfo.operator = 'add';
    dragInfo.type = evt.target.id;
}

function dragMove(evt){
    evt.preventDefault();
    this.style.opacity = 0.8;
    dragInfo.operator = 'move';
    dragInfo.target = evt.target.id;
}

function drop(evt) {
    evt.preventDefault();
    switch (dragInfo.operator){
        case 'add':
            var pos = layoutPos(evt);
            createCtrl(dragInfo.type, pos);
            resizeCtrls();
            break;
        case 'move':
            swapCtrl(dragInfo.target,evt.toElement.id);
            break;
    }
    return false;
}

function swapCtrl(srcId,dstId) {
    var src = document.getElementById(srcId);
    var dst = document.getElementById(dstId);
    src.style.opacity = 1;
    if( dst != src && dst.parentNode === src.parentNode ){
        var parent = src.parentNode;
        var sibling = parent.children;
        var srcIndex = -1;
        var dstIndex = -1;
        for(var i = 0; i < sibling.length; ++i){
            if(sibling[i] == src)
                srcIndex = i;
            else if(sibling[i] == dst)
                dstIndex = i;
            if(dstIndex != - 1 && srcIndex != -1)
                break;
        }

        if(dstIndex == - 1 || srcIndex == -1)
            return;

        if(dstIndex - srcIndex == -1) {
            parent.insertBefore(src, dst);
            return;
        }

        if(dstIndex - srcIndex == 1) {
            parent.insertBefore(dst, src);
            return;
        }

        if(srcIndex < dstIndex){
            var dstPos = src.nextSibling;
            parent.insertBefore(src,dst);
            parent.insertBefore(dst,dstPos);
        } else {
            var dragPos = dst.nextSibling;
            parent.insertBefore(dst,src);
            parent.insertBefore(src,dragPos);
        }
    }
}

function createCtrl(type, pos){
    var view = layout[pos.row].box;
    var ele = document.createElement('canvas');
    ele.id = type.substr(2) + count++;
    ele.style.float = 'left';
    ele.draggable = true;
    ele.ondrag = dragMove;

    if(pos.col == layout[pos.row].items.length)
        view.appendChild(ele);
    else
        view.insertBefore(ele, layout[pos.row].items[pos.col].holder());

    var ctrl = new window[type](ele.id);
    ctrl.setName(ele.id);
    ctrl.setLabel(ele.id);

    layout[pos.row].items.push(ctrl);
}

function resizeCtrls(){
    if(layout.length == 0)
        return;

    var mainView = document.getElementById('mainView');
    var i,j;
    var autoSize = [];
    var fixedRowCount = 0;

    var restWidth = mainView.width;
    var restHeight = mainView.height;

    for(i = 0; i < layout.length; ++i){
        var items = layout[i].items;
        var count = 0;
        var rowHeight = items[0].getSize()[1];
        restWidth = mainView.width;
        for(j = 0; j < items.length; ++j){
            var item = items[j];
            if(!item.fixedSize())
                continue;
            count += 1;
            var size = item.getSize();
            if(currCtrl && currCtrl.getName() == item.getName())
                rowHeight = size[1];
            restWidth -= size[0];
        }
        autoSize.push({
            autoWidth:parseInt(mainView.width / items.length),
            rowHeight: -1
        });
        if(!count)
            continue;
        autoSize[i].rowHeight = rowHeight;
        if(count != items.length)
            autoSize[i].autoWidth = parseInt(restWidth / (items.length - count));
        fixedRowCount += 1;
        restHeight -= rowHeight;
    }

    var autoHeight = mainViewBaseHeight / layout.length;
    if(fixedRowCount > 0 && fixedRowCount != layout.length)
        autoHeight = restHeight / (layout.length - fixedRowCount);
    if(autoHeight < 240)
        autoHeight = 240;
    var fixedHeight = mainView.height - restHeight;
    var realHeight = fixedHeight + autoHeight * (layout.length - fixedRowCount);
    if (realHeight < mainViewBaseHeight)
        realHeight = mainViewBaseHeight;
    mainView.height = realHeight;
    mainView.style.height = mainView.height + 'px';
    for (i = 0; i < autoSize.length; ++i) {
        if (autoSize[i].rowHeight != -1)
            continue;
        autoSize[i].rowHeight = autoHeight;
    }

    for(i = 0; i < layout.length; ++i){
        var ctrls = layout[i].items;
        for(j = 0; j < ctrls.length; ++j){
            var width = autoSize[i].autoWidth;
            var height = autoSize[i].rowHeight;
            var ctrl = ctrls[j];
            if(ctrl.fixedSize())
                width = ctrl.getSize()[0]
            ctrl.resize(width, height, ctrl.fixedSize());
        }
    }
}

function addBox() {
    var view = document.getElementById('mainView');
    var div = document.createElement('div');
    div.id = 'row' + layout.length;
    div.style.width = view.width + 'px';
    div.style.height = 'auto';
    layout.push({box:div, items:[]});
    view.appendChild(div);
}

function layoutPos(evt) {
    var res = {row:0, col:0};
    if(layout.length == 0){
        addBox();
        return res;
    }
    var view = document.getElementById('mainView');
    var len = layout.length;

    if(evt.pageY > view.height * (len - 0.5) / len){
        addBox();
        res.row = layout.length - 1;
        return res;
    }
    res.row = parseInt(evt.pageY * len / view.height + 0.5);
    res.col = parseInt(evt.pageX * layout[res.row].items.length / view.width - 0.5);
    return res;
}

function findCtrl(name){
    for(var i = 0; i < layout.length; ++i){
        var row = layout[i].items;
        for(var j = 0; j < row.length; ++j){
            if( row[j].getName() == name )
                return row[j];
        }
    }
    return null;
}

function delCtrl(){
    if(!currCtrl)
        return;
    var row = -1;
    var col = -1;
    for( var i = 0; i < layout.length; ++i){
        var row = layout[i].items;
        for(var j = 0; j < row.length; ++j){
            if( row[j] == currCtrl ) {
                row = i;
                col = j;
                break;
            }
        }
        if(col != -1)
            break;
    }
    if(row != -1 && col != -1) {
        var ctrl = document.getElementById(currCtrl.getName());
        var view = layout[row].box;

        view.removeChild(ctrl);
        layout[row].items.splice(col,1);

        if(layout[row].items.length == 0){
            var mainView = document.getElementById('mainView');
            mainView.removeChild(view);
            layout.splice(row,1);
        }
        resizeCtrls();
    }

    hideConfigPage();
}

function showConfigPage(){
    var mainView = document.getElementById('mainView');
    var confPage = document.getElementById('configPage');
    var url = document.getElementById('configSrcUrl');
    var interval = document.getElementById('configUpdateInterval');
    var name = document.getElementById('configCtrlName');
    var width = document.getElementById('configCtrlWidth');
    var height = document.getElementById('configCtrlHeight');
    var size = currCtrl.getSize();
    url.value = currCtrl.getUrl();
    name.value = currCtrl.getLabel();
    interval.value = currCtrl.getInterval() / 1000;
    width.value = size[0];
    height.value = size[1];
    confPage.style.display = 'block';
    var top = (mainViewBaseHeight - confPage.offsetHeight) * 0.5;
    confPage.style.top =  top + window.pageYOffset + 'px';
    confPage.style.left = (window.innerWidth - confPage.offsetWidth) * 0.5 + 'px';
    console.log( top, mainView.scrollHeight);
}

function hideConfigPage(){
    var confPage = document.getElementById('configPage');
    confPage.style.display = 'none';
    currCtrl = null;
    return false;
}

function updateConfig(){
    var url = document.getElementById('configSrcUrl').value;
    var interval = document.getElementById('configUpdateInterval').value;
    var label = document.getElementById('configCtrlName').value;
    var width = document.getElementById('configCtrlWidth').value;
    var height = document.getElementById('configCtrlHeight').value;
    var prefix = 'http://'+window.location.host+'/';
    var size = currCtrl.getSize();
    var sizeChanged = size[0] != width || size[1] != height;
    var fixed = width > 0 && height > 0;
    if(url && url.indexOf(prefix) == -1)
        url = prefix + url;
    currCtrl.setUrl(url);
    currCtrl.setLabel(label);
    currCtrl.setInterval(interval * 1000);
    currCtrl.resize(width, height, sizeChanged && fixed);
    if(sizeChanged)
        resizeCtrls();
    hideConfigPage();
}

function contextMenu(e) {
    currCtrl = findCtrl(e.toElement.id);
    showConfigPage();
    return false;
}

(function(){
    resize();
    var mainView = document.getElementById('mainView');
    mainView.ondblclick = contextMenu;
}());

