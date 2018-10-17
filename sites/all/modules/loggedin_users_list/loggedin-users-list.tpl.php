<?php

/**
 * @file
 * Show logged-in user list.
 */
?>
<table> <tr> <td><b>Currently number of logged-in users:</b></td> <td><b><?php print check_plain($users_no);?></b></td> </tr> </table>
<?php print $userdata;?>
