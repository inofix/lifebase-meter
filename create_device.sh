#!/bin/bash
if [ $1 == debug ] ; then
    set -x
    shift
fi
version="0.5"
#** Version: 0.5
#*  This script prepares a custom configuration for a
#* certain LifeBase setup with an ESP32 in the `configs`
#* folder and prepares then the loadable code under
#* the `build` folder.
#*  To change a preexisting configuration file, please
#* remove it first as we will not overwrite existing
#* values.

# configurable stuff
lifebaseprefix="lifebase_meter"

configexamplename="${lifebaseprefix}-.conf.stub"
configsdir="configs"

codedir="arduino-ide/$lifebaseprefix"
mainfile="$codedir/$lifebaseprefix.ino"
builddir="build"

# connectivity
ble="on"
wifi="off"
mqtt="off"

wifi_ssid=""
wifi_password=""

mqtt_broker=""
mqtt_port="1883"
mqtt_namespace=""
mqtt_user=""
mqtt_password=""

# default service settings
lightservce="on"
luxmeter="2591"

airservice="on"
waterservice="on"
soilservice="on"
extraservice="off"

pump_mode=0

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

while true ; do
    case "$1" in
#*      -a/-A               turn airservice (on)/off
        -a)
            airservice="on"
        ;;
        -A)
            airservice="off"
        ;;
#*      --ble enable        turn BLE (on)/off
        --ble)
            shift
            if [ "$1" == "on" ] || [ "$1" == "off" ] ; then
                ble="$1"
            else
                echo "Please set '--ble' to either 'on' or 'off'"
                exit 1
            fi
        ;;
#*      -e|-E               turn extraservice on/(off)
        -e)
            extraservice="on"
        ;;
        -E)
            extraservice="off"
        ;;
#*      -l/-L               turn lightservice (on)/off
        -l)
            lightservice="on"
        ;;
        -L)
            lightservice="off"
        ;;
#*      --luxmeter sensor   choose a sensor model, currently supported are:
#*                          2561, 2591
        --luxmeter)
            shift
            luxmeter="$1"
        ;;
#*      --mqtt enable       turn mqtt on/(off)
        --mqtt)
            shift
            if [ "$1" == "on" ] ; then
                wifi="on"
                mqtt="on"
            elif [ "$1" == "off" ] ; then
                mqtt="off"
            else
                echo "Please set '--mqtt' to either 'on' or 'off'"
                exit 1
            fi
        ;;
#*      --pump-on/--pump-int
#*      -p/-P/              pump continuously or in intervals
        -p|--pump-on)
            pump_mode=0
        ;;
        -P|--pump-int)
            pump_mode=1
        ;;
#*      -s/-S               turn soilservice (on)/off
        -s)
            soilservice="on"
        ;;
        -S)
            soilservice="off"
        ;;
#*      -w/-W               turn waterservice (on)/off
        -w)
            waterservice="on"
        ;;
        -W)
            waterservice="off"
        ;;
#*      --wifi enable       turn wifi on/(off)
        --wifi)
            shift
            if [ "$1" == "on" ] || [ "$1" == "off" ] ; then
                wifi="$1"
            else
                echo "Please set '--wifi' to either 'on' or 'off'"
                exit 1
            fi
        ;;
#*      -v | --version      print version information
        -v|--version)
            grep "^#\*\* " $0 | sed 's;^#\*\*;;'
            exit 0
        ;;
#*      -h | --help         print this help
        -h|--help)
            grep "^#\* " $0 | sed -e 's;^#\*;;' -e 's;$0;'$0';'
            exit 0
        ;;
        -*)
            grep "^#\* " $0 | sed -e 's;^#\*;;' -e 's;$0;'$0';'
            exit 1
        ;;
        ?*)
            s=( ${1/:/ } )
            subject[i]=${s[0]}
            let i++
            subject[i]=${s[1]}
            let i++
        ;;
        *)
            break
        ;;
    esac
    shift
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

##TODO split this tool ...

# here we create a config file per device
if [ -f $configfilename ] ; then
    cfg_version=$(grep "version=" $configfilename)
    cfg_version=${cfg_version#config_version=}
    cfg_version=${cfg_version//\"/}
    if [ "$version" != "$cfg_version" ] ; then
        echo "Error: the version of the preexisting config file is not compatible with this script, please merge manually"
    fi
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
        for f in $configsdir/* ; do
            grep 'SUBJECT_TYPE_NAME="'"$subjecttypename"'"' $f ; retval=$?
            if [ $retval -eq 0 ] ; then
                s=$(grep 'SUBJECT_TYPE_UUID=' $f)
                subjecttypeuuid=${s#SUBJECT_TYPE_UUID=}
                subjecttypeuuid=${subjecttypeuuid//\"/}
                echo "I found a config with this type already, it has the following type-UUID: $subjecttypeuuid"
            fi
        done
        if [ "$subjecttypeuuid" == "" ] ; then
            subjecttypeuuid=$(uuidgen)
        fi
        echo "Please provide a UUID for the type of this device [$subjecttypeuuid]:"
        read a
        if [ -n "$a" ] ; then
            subjecttypeuuid=$a
        fi
    fi
    if [ "$wifi" == "on" ] ; then
        echo "Please provide the SSID for your WiFi network:"
        read a
        if [ -n "$a" ] ; then
            wifi_ssid="$a"
        fi
        echo "Please provide the PSK for your WiFi network:"
        read a
        if [ -n "$a" ] ; then
            wifi_password="$a"
        fi
    fi
    if [ "$mqtt" == "on" ] ; then
        echo "Please provide the address for your MQTT broker:"
        read a
        if [ -n "$a" ] ; then
            mqtt_broker="$a"
        fi
        echo "Please provide the port for your MQTT broker [$mqtt_port]:"
        read a
        if [ -n "$a" ] ; then
            mqtt_port="$a"
        fi
        echo "Please provide the namespace for your MQTT topic"
        read a
        if [ -n "$a" ] ; then
            mqtt_namespace="$a"
        fi
        echo "Please provide the user to connect to your MQTT broker"
        read a
        if [ -n "$a" ] ; then
            mqtt_user="$a"
        fi
        echo "Please provide the password to connect to your MQTT broker"
        read a
        if [ -n "$a" ] ; then
            mqtt_password="$a"
        fi
    fi
    sed -e 's/SUBJECT_NAME=""/SUBJECT_NAME="'"$subjectname"'"/' \
            -e 's/SUBJECT_UUID=""/SUBJECT_UUID="'$subjectuuid'"/' \
            -e 's/SUBJECT_TYPE_NAME=""/SUBJECT_TYPE_NAME="'"$subjecttypename"'"/' \
            -e 's/SUBJECT_TYPE_UUID=""/SUBJECT_TYPE_UUID="'$subjecttypeuuid'"/' \
            -e 's/LIGHT_SERVICE=""/LIGHT_SERVICE="'$lightservce'"/' \
            -e 's/AIR_SERVICE=""/AIR_SERVICE="'$airservice'"/' \
            -e 's/WATER_SERVICE=""/WATER_SERVICE="'$waterservice'"/' \
            -e 's/PUMP_MODE=[0-9]*/PUMP_MODE='$pump_mode'/' \
            -e 's/SOIL_SERVICE=""/SOIL_SERVICE="'$soilservice'"/' \
            -e 's/EXTRA_SERVICE=""/EXTRA_SERVICE="'$extraservice'"/' \
            -e 's/BLE_ENABLE=".*"/BLE_ENABLE="'$ble'"/' \
            -e 's/WIFI_ENABLE=".*"/WIFI_ENABLE="'$wifi'"/' \
            -e 's/WIFI_DEFAULT_SSID=""/WIFI_DEFAULT_SSID="'$wifi_ssid'"/' \
            -e 's/WIFI_DEFAULT_PASSWORD=""/WIFI_DEFAULT_PASSWORD="'$wifi_password'"/' \
            -e 's/WIFI_MQTT_ENABLE=".*"/WIFI_MQTT_ENABLE="'$mqtt'"/' \
            -e 's/WIFI_MQTT_DEFAULT_BROKER=""/WIFI_MQTT_DEFAULT_BROKER="'$mqtt_broker'"/' \
            -e 's/WIFI_MQTT_DEFAULT_PORT=""/WIFI_MQTT_DEFAULT_PORT="'$mqtt_port'"/' \
            -e 's/WIFI_MQTT_DEFAULT_NAMESPACE=""/WIFI_MQTT_DEFAULT_NAMESPACE="'$mqtt_namespace'"/' \
            -e 's/WIFI_MQTT_DEFAULT_USER=""/WIFI_MQTT_DEFAULT_USER="'$mqtt_user'"/' \
            -e 's/WIFI_MQTT_DEFAULT_PASSWORD=""/WIFI_MQTT_DEFAULT_PASSWORD="'$mqtt_password'"/' \
                $configexamplename > $configfilename
fi

# here we generate the code for each device
if [ -f "$configfilename" ] ; then

    declare -A vars

    oIFS=$IFS
    IFS="
"
    for l in $(awk '/^[_0-9a-zA-Z]**=.*$/ {print $0}' $configfilename) ; do
        k=${l%=*}
        v=${l##*=}
        vars[$k]="$v"
    done
    IFS=$oIFS

    if [ -d "$builddir/$lifebaseprefix-$subjectname" ] ; then
        rm $builddir/$lifebaseprefix-$subjectname/*
        rmdir $builddir/$lifebaseprefix-$subjectname
    fi
    mkdir $builddir/$lifebaseprefix-$subjectname
    cp $codedir/*.ino $builddir/$lifebaseprefix-$subjectname/
    mv $builddir/$lifebaseprefix-$subjectname/${mainfile##*/} $builddir/$lifebaseprefix-$subjectname/$lifebaseprefix-$subjectname.ino
    for u in WIFI BLE ; do
        s=${u}_ENABLE
        if [ "${vars[$s]}" == "on" ] ; then
            sed -i -e 's;//#define '$u';#define '$u';' \
                $builddir/$lifebaseprefix-$subjectname/$lifebaseprefix-$subjectname.ino
        fi
    done
    for s in LIGHT_SERVICE AIR_SERVICE WATER_SERVICE SOIL_SERVICE EXTRA_SERVICE ; do
        u=${s}_UUID
        if [ "${vars[$s]}" == "on" ] ; then
            sed -i -e 's;//#define '$u';#define '$u';' \
                $builddir/$lifebaseprefix-$subjectname/$lifebaseprefix-$subjectname.ino
        fi
    done
    # possible delimiters for `sed` in case one is already used in the string itself:
    ds=( "|" ";" ":" "," "." "#" "~" "%" "&" "/" "-" "+" "=" )
    for k in ${!vars[@]} ; do
        for d in ${ds[@]} ; do
            # test each against the value and break on the first negative match
            if [ "${vars[$k]/*$d*/XXX}" != "XXX" ] || [ "${vars[$k]}" == "XXX" ] ; then
                break
            fi
        done
        sed -i -e "s$d"'^\([ ]*\)#define '"${k}"' .*'"$d"'\1#define '"${k} ${vars[$k]}$d" \
                $builddir/$lifebaseprefix-$subjectname/$lifebaseprefix-$subjectname.ino
    done
fi

