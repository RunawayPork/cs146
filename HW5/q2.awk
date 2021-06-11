#---2.1.a---
./awkcel
'BEGIN{ FS="\t" } 
{      
    for (x=1; x<=NF; x++){
        if (NR==1){
            species[x] = $x;
        }          
        if ($x=="_"){
            continue;
        }          
        for (y=x+1; y<=NF; y++){
            if ($y=="_"){
                continue;
            }              
        arr[x,y]++;          
        }      
    }  
}
END{
    smol1="";
    smol2="";
    chonky1="";
    chonky2="";
    for (x=1; x<=NF; x++){
        for (y=x+1; y<=NF; y++){
            if (smol1==""){
                smol1=x;                  
                smol2=y;
            }              
            else if (arr[x,y] < arr[smol1,smol2]){
                smol1=x;                  
                smol2=y;
            }              
            if(chonky1==""){
                chonky1=x;
                chonky2=y;
            }              
            else if(arr[x,y] > arr[chonky1,chonky2]){
                chonky1=x;                  
                chonky2=y;
            }          
        }      
    }      
    printf "[%s,%s] share have the most in common with %s orthologs.\n", species[chonky1], species[chonky2], arr[chonky1,chonky2];
    printf "[%s,%s] share have the least in common with %s orthologs.\n", species[smol1], species[smol2], arr[smol1,smol2];
}' orthologs.csv 


#---2.1.b---
./awkcel 
'BEGIN{ FS="\t" }
{
    for (x=1; x<=NF; x++){         
        if (NR==1){              
            species[x] = $x;          
        }          
        if ($x=="_"){              
            continue;
        }
        for (y=x+1; y<=NF; y++){
            if ($y=="_"){
                continue;
            }              
            arr[x,y]++;          
        }      
    }  
}  
END{
    for (x=1; x<=NF; x++){
        for (y=x+1; y<=NF; y++){
            printf "[%s,%s] share %s orthologs.\n", species[x], species[y], arr[x,y];
        }    
    }  
}' orthologs.csv


#---2.2.a---
./awkcel 
'BEGIN{ FS="\t" }
{        
    if (date=="2011-07-01"){
        print "2011-07-01";
        print "\tAmazon = ", AMZN;
        print "\tMicrosoft = ", MSFT;
        print "";
    }     
    else if(date=="2011-07-05"){
        print "2011-07-05";
        print "\tAmazon = ", AMZN;
        print "\tMicrosoft = ", MSFT;
    }
}' historical.2011.tsv 


#---2.2.b---
./awkcel
'BEGIN{ FS="\t" }
{
    if (NR<=1){
        prevStock = AMZN;
    }     
    else{
        delta = AMZN-prevStock;
        if (delta<notStonks){
            d=date;             
            notStonks=delta;
        }         
        prevStock=AMZN;
    } 
} 
END{
    print d, notStonks;
}' historical.2011.tsv


#---2.2.c---
./awkcel 
'BEGIN{ FS="\t" }
{     
    for (i=2; i<NF; i++){
        if (NR==1){
            arr[i]["company"] = $i;
        }         
        if (NR==2){
            arr[i]["prevStock"] = $i;
            arr[i]["lossPct"] = "";
        }         
        else{
            if (arr[i]["prevStock"] != "_" && $i != "_"){
                delta = $i-arr[i]["prevStock"];
                if (delta<0){                     
                    lossPct=delta/arr[i]["prevStock"];
                    if (arr[i]["lossPct"] == "" || lossPct < arr[i]["lossPct"]){
                        arr[i]["lossPct"]=lossPct;                         
                        arr[i]["day"]=date;
                    }                 
                }             
            }             
        arr[i]["prevStock"] = $i;
        }     
    } 
} 
END{
    notStonks= "";
    for (i=2;i<NF;i++){
        if (notStonks=="" && arr[i]["day"] != ""){
            notStonks=i;
        }         
        else if (arr[i]["lossPct"] < arr[notStonks]["lossPct"] && arr[i]["day"] != ""){
            notStonks=i;
        }     
    }     
    print arr[notStonks]["company"], "\t", arr[notStonks]["day"], "\t",arr[notStonks]["lossPct"]*100,"%";
}' historical.2011.tsv 


#Spiral Galaxies Questions
./awkcel
'BEGIN{ FS="\t" }
{
    P_SP = P_CW + P_ACW;
    if (NR!=1){
        if (P_SP == 0){
        zero++;
        }
        if (P_SP == 1){
            one++;
        }
        if (P_SP > 0.5){
            greaterThanHalf++;
        }
    }
}
END{
    printf "There are %s galaxies that have P_SP = 0.\n", zero;
    printf "There are %s galaxies that have P_SP > 0.5.\n", greaterThanHalf;
    printf "There are %s galaxies that have P_SP = 1.\n", one;
}' /home/wayne/pub/cs146/awkcel/SDSS+GZ1+SpArcFiRe+SFR.tsv


#Histogram
./awkcel '{
    P_SP=P_CW+P_ACW;
    bin=int(10*P_SP);
    n[bin]++;
    numArcs[bin]+=totalArcLength;
}
END{
    for(i=0;i<=10;i++)
    print i, n[i], numArcs[i]/n[i];
}' /home/wayne/pub/cs146/awkcel/SDSS+GZ1+SpArcFiRe+SFR.tsv