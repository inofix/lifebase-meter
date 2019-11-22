#!/bin/bash
#** Version: 0.2
#* This script prepares a custom configuration for a
#* certain LifeBase setup with an ESP32.
#* To change a preexisting configuration file, please
#* remove it first as we will not overwrite existing
#* values.

# configurable stuff
lifebaseprefix="lifebase_meter"

configexamplename="$lifebaseprefix-example.conf"
configsdir="configs"

codedir="arduino-ide/$lifebaseprefix"
mainfile="$codedir/$lifebaseprefix.ino"
builddir="build"

# default service settings
lightservce="on"
airservice="on"
waterservice="on"
soilservice="on"

# helper vars
subject=()
i=0

# starting actions
mkdir -p $configsdir $builddir

if [ ! -f "$configexamplename" ] ; then
    echo "Error: no example config file found ($configexamplename)"
    exit 1
fi 
if [ ! -f "$mainfile" ] ; then
    echo "Error: no program code file found ($mainfile)"
    exit 1
fi 

#*  usage: $0 [options] [subjectname[:subjectuuid] [subjecttypename[:subjecttypeuuid]]]
#*    options:

for s in $@ ; do
    case $1 in
#*      -a/-A               turn airservice on/off
        -a)
            airservice="on"
        ;;
        -A)
            airservice="off"
        ;;
#*      -l/-L               turn lightservice on/off
        -l)
            lightservice="on"
        ;;
        -L)
            lightservice="off"
        ;;
#*      -s/-S               turn waterservice on/off
        -s)
            soilservice="on"
        ;;
        -S)
            soilservice="off"
        ;;
#*      -w/-W               turn waterservice on/off
        -w)
            waterservice="on"
        ;;
        -W)
            waterservice="off"
        ;;
#*      -v | --version      print version information
        -v|--version)
            grep "^#\*\* " $0 | sed 's;^#\*\*;;'
            exit 0
        ;;
#*      -h | --help         print this help
        -*)
            grep "^#\* " $0 | sed 's;^#\*;;'
            exit 0
        ;;
        -*)
            grep "^#\* " $0 | sed 's;^#\*;;'
            exit 1
        ;;
        ?*)
            s=( ${1/:/ } )
            subject[i]=${s[0]}
            let i++
            subject[i]=${s[1]}
            let i++
            shift
        ;;
    esac
done

subjectname=${subject[0]}
subjectuuid=${subject[1]}
subjecttypename=${subject[2]}
subjecttypeuuid=${subject[3]}

if [ -z "$subjectname" ] ; then
    subjectname="Development0"
    echo "Please provide a name for this device [$subjectname]:"
    read a
    if [ -n "$a" ] ; then
        subjectname=$a
    fi
fi

configfilename="$configsdir/lifebase-meter-$(echo $subjectname | sed 's;\s;;g').conf"

if [ -f $configfilename ] ; then
    if [ -n "$subjectuuid$subjecttype$subjecttypeuuid" ] ; then
        echo "Error: a configuration file already existed but you provided additional infos - exiting"
        exit 1
    fi
else
    if [ -z "$subjectuuid" ] ; then
        subjectuuid=$(uuidgen)
        echo "Please provide a UUID for this device [$subjectuuid]:"
        read a
        if [ -n "$a" ] ; then
            subjectuuid=$a
        fi
    fi
    if [ -z "$subjecttypename" ] ; then
        subjecttypename="Development"
        echo "Please provide a type for this device [$subjecttypename]:"
        read a
        if [ -n "$a" ] ; then
            subjecttypename=$a
        fi
    fi
    if [ -z "$subjecttypeuuid" ] ; then
        subjecttypeuuid=$(uuidgen)
        echo "Please provide a UUID for the type of this device [$subjecttypeuuid]:"
        read a
        if [ -n "$a" ] ; then
            subjecttypeuuid=$a
        fi
    fi
    sed -e 's/SUBJECT_NAME=""/SUBJECT_NAME="'$subjectname'"/' \
            -e 's/SUBJECT_UUID=""/SUBJECT_UUID="'$subjectuuid'"/' \
            -e 's/SUBJECT_TYPE_NAME=""/SUBJECT_TYPE_NAME="'$subjecttypename'"/' \
            -e 's/SUBJECT_TYPE_UUID=""/SUBJECT_TYPE_UUID="'$subjecttypeuuid'"/' \
            -e 's/LIGHT_SERVICE=""/LIGHT_SERVICE="'$lightservce'"/' \
            -e 's/AIR_SERVICE=""/AIR_SERVICE="'$airservice'"/' \
            -e 's/WATER_SERVICE=""/WATER_SERVICE="'$waterservice'"/' \
            -e 's/SOIL_SERVICE=""/SOIL_SERVICE="'$soilservice'"/' \
                $configexamplename > $configfilename
fi

if [ -f "$configfilename" ] ; then

    vars=( $(awk '/^[_0-9a-zA-Z]**="[-_+#0-9a-zA-Z]*"$/ {print $0}' $configfilename) )

    for v in ${vars[@]} ; do
        eval $v
    done

    if [ -d "$builddir/$lifebaseprefix-$SUBJECT_NAME" ] ; then
        rm $builddir/$lifebaseprefix-$SUBJECT_NAME/*
        rmdir $builddir/$lifebaseprefix-$SUBJECT_NAME
    fi
    cp -r $codedir $builddir/$lifebaseprefix-$SUBJECT_NAME
    mv $builddir/$lifebaseprefix-$SUBJECT_NAME/${mainfile##*/} $builddir/$lifebaseprefix-$SUBJECT_NAME/$lifebaseprefix-$SUBJECT_NAME.ino
    for s in LIGHT_SERVICE AIR_SERVICE WATER_SERVICE SOIL_SERVICE ; do
        u=${s}_UUID
        if [ "${!s}" == "on" ] ; then
            sed -i -e 's;//#define '$u';#define '$u';' \
                $builddir/$lifebaseprefix-$SUBJECT_NAME/$lifebaseprefix-$SUBJECT_NAME.ino
        fi
    done
    for v in ${vars[@]} ; do
        sed -i -e 's;^#define '${v%=*}' .*;#define '${v%=*}' '${v#*=}';' \
                $builddir/$lifebaseprefix-$SUBJECT_NAME/$lifebaseprefix-$SUBJECT_NAME.ino
    done
fi

