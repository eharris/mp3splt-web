<?php

include 'menu.php';

$os_version=$_GET['version'];

$image="debian.png";
$debian_or_ubuntu="debian";

$is_ubuntu=$_GET['ubuntu'];
if ($is_ubuntu)
{
  $image="ubuntu.png";
  $debian_or_ubuntu="ubuntu";
}

$debian_or_ubuntu_upper = ucwords($debian_or_ubuntu);
$os_version_lower = strtolower($os_version);

function create_main_page()
{
  include 'download_variables.php';

  global $os_version, $os_version_lower, $image, $debian_or_ubuntu, $debian_or_ubuntu_upper;

  if (!array_key_exists($os_version, $debian_versions) &&
      !array_key_exists($os_version, $ubuntu_versions))
  {
    echo "<br/>No package for version $os_version.<br/><br/>";
    return;
  }

  echo "
<h2  class=\"pagetitle\">$debian_or_ubuntu_upper $os_version packages</h2>
<hr />";


  echo "<div class=\"title\">From repository:</div>";


  echo "<ol><li>Add the repositories:<br/>";
  echo "<code>add-apt-repository \"deb http://mp3splt.sourceforge.net/repository $os_version_lower main\"</code></li>";

  echo "<li>Update the packages:<br />";
  echo "<code>apt-get update</code></li>";

  echo "<li>Install mp3splt and mp3splt-gtk:<br/>";
  echo "<code>apt-get install libmp3splt0-mp3 libmp3splt0-ogg libmp3splt0-flac libmp3splt-doc libmp3splt-dev mp3splt mp3splt-gtk</code></li>";
  echo "</ol>";

  echo "<div class=\"title\">Manual:</div><br />";

echo "<br/><table class=\"mainpagedownloadtable\">

<!-- top stuff -->
<tr>
<td style=\"text-align:center\">
<img alt=\"\" src=\"icons/$image\" /><br />
</td>
<td class=\"mainpagedownloadtd\" style=\"border-top:none;text-align:center\">
<div style=\"font-weight:bold\">Libmp3splt</div>
<div style=\"font-style:italic\">${libmp3splt_version}</div>
</td>
<td class=\"mainpagedownloadtd\" style=\"border-top:none;text-align:center\">
<div style=\"font-weight:bold\">Mp3splt</div>
<div style=\"font-style:italic\">${mp3splt_version}</div>
</td>
<td class=\"mainpagedownloadtd\" style=\"border-top:none;text-align:center;border-right:none\">
<div style=\"font-weight:bold\">Mp3splt-gtk</div>
<div style=\"font-style:italic\">${mp3splt_gtk_version}</div>
</td>
</tr>";

echo "<tr id=\"$debian_or_ubuntu\">";

$architectures=array("i386", "amd64");

foreach ($architectures as $arch) {

  $libmp3splt_suffix="${libmp3splt_version}.${os_version_lower}_$arch.deb${download_suffix}";
  $mp3splt_suffix="${mp3splt_version}.${os_version_lower}_$arch.deb${download_suffix}";
  $mp3splt_gtk_suffix="${mp3splt_gtk_version}.${os_version_lower}_$arch.deb${download_suffix}";

  echo "<td class=\"mainpagedownloadtd\" style=\"border-bottom:none;border-left:none;text-align:center;border-top:solid;border-top-width:1pt;\">
<div class=\"osname\">$arch</div>
</td>
<td class=\"mainpagedownloadtd\" style=\"border-bottom:none;border-top:solid;border-top-width:1pt;\">
  <a href=\"${download_url}libmp3splt0_${libmp3splt_suffix}\">libmp3splt0</a><br />
  <a href=\"${download_url}libmp3splt0-mp3_${libmp3splt_suffix}\">libmp3splt0-mp3</a><br />
  <a href=\"${download_url}libmp3splt0-ogg_${libmp3splt_suffix}\">libmp3splt0-ogg</a><br />
  <a href=\"${download_url}libmp3splt0-flac_${libmp3splt_suffix}\">libmp3splt0-flac</a><br />
  <hr style=\"border: none\" />
  <a href=\"${download_url}libmp3splt-dev_${libmp3splt_suffix}\">libmp3splt-dev</a><br />
  <a href=\"${download_url}libmp3splt-doc_${libmp3splt_suffix}\">libmp3splt-doc</a><br />
</td>

<td class=\"mainpagedownloadtd\" style=\"border-bottom:none;border-top:solid;border-top-width:1pt;\">
  <a href=\"${download_url}mp3splt_${mp3splt_suffix}\">mp3splt</a><br />
</td>

<td class=\"mainpagedownloadtd\" style=\"border-bottom:none;border-right:none;border-top:solid;border-top-width:1pt;\">
  <a href=\"${download_url}mp3splt-gtk_${mp3splt_gtk_suffix}\">mp3splt-gtk</a><br />
</td>

</tr>";

}

echo "</tr></table><br />";

echo "libmp3splt0 - main library needed for both mp3splt and mp3splt-gtk<br /><br />";

echo "libmp3splt0-mp3 - mp3 plugin<br />";
echo "libmp3splt0-ogg - ogg vorbis plugin<br />";
echo "libmp3splt0-flac - native FLAC plugin<br /><br />";

echo "libmp3splt-dev - development package<br />";
echo "libmp3splt-doc - libmp3splt API documentation<br /><br/>";

echo "mp3splt - command line client<br /><br/>";
echo "mp3splt-gtk - graphical user interface client<br /><br/>";

}

begin_document("mp3splt project - $debian_or_ubuntu downloads", "default.css",FALSE);

create_table_with_menu("downloads");

end_document();

?>

