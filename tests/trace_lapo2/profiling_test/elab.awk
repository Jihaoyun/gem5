#!/usr/bin/awk -f

{
    split($2,vector,"p")
    if ( vector[1] >= 8573560000 && vector[1] <= 11489339000 )
        print $0 >> "elab_out.txt";

}
