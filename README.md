# hashi-vita
A simple port of the game Hashi to the PS Vita.&nbsp; Not a perfect port but it satisfies my desire to play Hashi on the go.

<p align="center">
  <img src="https://raw.githubusercontent.com/samobot/hashi-vita/main/hashiscreenshot.png" width="70%" height="auto"/>
</p>

## Install
Download the newest [release](https://github.com/samobot/hashi-vita/releases) and install the VPK using VitaShell or another package installer

## Build
Compile using make or using vitasdk manually. \
\
If VitaShell's FTP server is enabled, you can use `PSVITAIP="(insert device ip)" make vpksend` to send the VPK to your Vita automatically.&nbsp; It will place the file in the `ux0:` root directory. \
\
Requires vitasdk libraries and toolchain to be installed.

## Built with
* [vitasdk](https://vitasdk.org/)

## Author
Samobot

## License
[GPLv3](https://www.gnu.org/licenses/gpl-3.0.txt)
