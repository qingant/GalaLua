
function trigeEdit(self)
{
    //alert(thisform.innerHTML);
    
    if (self.innerHTML == "提交")
    {
        return true;
    }
    self.innerHTML = "提交"; 
    var father = self.parentNode;
    $("input[disabled]").each( function () {this.disabled = false;});
    $("select[disabled]").each( function () {this.disabled = false;} );
    $("button[disabled]").each( function () {this.disabled = false;} );
    // this.name = "1234";
    return false;
};

function trigeDelete(usable, cat)
{
    document.getElementById(usable).value = "0";
    $.get("/cfg/groups/", {'path':usable, 'cat':cat}, function(){window.location.reload();});
    return true;
}

function trigeChange(self)
{
    var parent = self.parentNode;
    // alert(self.value);
    // alert(self.options);
    // alert(self.options.each);
    $("*", self).each(function (){
        var tag = this.innerHTML;


        if (self.getAttribute("bytag") == "0")
        {
            tag = this.value;
            
        }
        var value = this.value;
        //alert(tag);
        // alert("div[@enum="+tag+"]");
        if (value == self.value)
        {
         
            // alert( $("div[enum="+tag+"]", parent));
            //alert("DDDDDDD"+tag);
            $("div[enum="+tag+"]").each(function (){
                this.style.display = "block";
                $("*[name$=Usable]", this).each(function (){this.value = "1";});
            }
            );
        }
        else
        {
            $("div[enum="+tag+"]").each(function ()
                                                {
                                                    this.style.display = "none";
                                                    //$("*[name$=Usable]", this).each(function (){this.value = "1";});
                                                }
                                               );

        }
    }
    );
        
};


// function trigeSelect(id){
//     var select = document.getElementById(id);
//     var value = select.selectedIndex;
//     select.options.each(
//         {if (this == select.options[value]){
//             \$("#"+this).style.visibility = "visible";
//         }else{
//             \$("#"+this).style.display = "visible";
//         }});
// };
