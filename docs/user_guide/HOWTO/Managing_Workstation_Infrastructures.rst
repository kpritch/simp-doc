.. _Infrastructure-Setup:

HOWTO Manage Workstation Infrastructures
========================================

This chapter describes example code used to manage client workstations with a
SIMP system including GUIs, repositories, virtualization,
printing, and Virtual Network Computing (VNC).

Install Extra Puppet Modules
----------------------------

The examples on this page use modules that are part of SIMP Extras and may not
be installed on the puppet server by default.  The following is an example manifest
that can be applied to the puppet server to install the extra modules if RPMs are being
used to distribute the modules:


.. code-block:: ruby

   class site::workstation_packages {

     $package_list = [
       'pupmod-simp-dconf',
       'pupmod-simp-gdm',
       'pupmod-simp-gnome',
       'pupmod-simp-simp_nfs',
       'pupmod-simp-vnc',
       'pupmod-simp-libvirt',
     ]

     package { $package_list :
       ensure => installed,
     }
   }

Create A Workstation Profile Class
----------------------------------

Below is an example class,
``/etc/puppetlabs/code/environments/simp/modules/site/manifests/workstation.pp``, that could be
set up a user workstation.  Each ``site::`` class is described in the subsequent sections.

.. code-block:: ruby

   class site::workstation {
     include 'site::gui'
     include 'site::repos'
     include 'site::virt'
     include 'site::print::client'

     # make sure any repos are installed before they
     # are needed.  Include dependencies to
     # other classes if needed.
     Class[Site::Repos] -> Class[Site::Gui]

     # Make sure everyone can log into all nodes.
     # If you want to change this, simply remove this line and add
     # individual entries to your nodes as appropriate
     pam::access::rule { "Allow Users":
       comment => 'Allow all users in the "users" group to access the system from anywhere.',
       users   => ['(users)'],
       origins => ['ALL']
     }

     # Install additional packages on the workstations.
     # Example list of General Use Packages
     package { [
       'pidgin',
       'vim-enhanced',
       'tmux',
       'git'
     ]: ensure => installed,
        require => Class[Site::Repos]
     }
   }


Workstation Repositories
^^^^^^^^^^^^^^^^^^^^^^^^

Create any repos needed to install extra software.


.. code-block:: ruby

   class site::repos {
     yumrepo { 'myrepo':
       #what ever parameters you need
     }
   }

.. _Graphical Desktop Setup:

Graphical Desktop Setup
^^^^^^^^^^^^^^^^^^^^^^^

The following is an example that may be used to set up a graphical desktop. The
class name assumes that the file has been placed at
``/etc/puppetlabs/code/environments/simp/modules/site/manifests/gui.pp``

.. code-block:: ruby

   class site::gui (
     Boolean $libreoffice = true
   ) {

     include 'gdm'
     include 'gnome'
     include 'vnc::client'
     # Browser and e-mail client are not installed by default.
     include 'mozilla::firefox'

     Class['Gnome'] -> Class['Site::gui']

     #SIMP gnome package provides a basic interface.
     #Add gnome extensions for the users.
     package { [
       'gnome-color-manager',
       'gnome-shell-extension-windowsNavigator',
       'gnome-shell-extension-alternate-tab',
       ]:
        ensure => installed,
     }

     #Gui applications
     if $libreoffice {
       package { 'libreoffice': ensure => installed }
     }
   }


Apply the Settings
------------------

Once the profiles have been created and tested, one way of applying the
profile to all workstations is to use the SIMP ``hostgroup`` :term:`Hiera`
configuration capability.

To do use ``hostgroups``, you will need to edit the ``site.pp`` in the target
environment's :term:`site manifest`.

Adding the following to
``/etc/puppetlabs/code/environments/simp/manifests/site.pp`` will will make all
nodes whose names start with ``ws`` followed by any number of digits use the
``hieradata/hostgroups/workstation.yaml``. All other nodes will fall back to
the ``default.yaml``.

.. code-block:: ruby

  case $facts['hostname'] {
    /^ws\d+.*/: { $hostgroup = 'workstation' }
    default:    { $hostgroup = 'default'     }
  }


The ``workstation.yaml`` file will include settings for all the workstations.

The following example includes the settings for NFS mounted home directories.
See  :ref:`Exporting Home Directories` for more information.

.. code-block:: yaml

  ---

  #Set the run level so it will bring up a graphical interface
  simp::runlevel: 'graphical'
  timezone::timezone: 'EST'

  #Settings for home server. See HOWTO NFS for more info.
  nfs::is_server: false
  simp_nfs::home_dir_server: myhome.server.com

  #The site::workstation manifest will do most of the work.
  classes:
    - site::workstation
    - simp_nfs


Virtualization on User Workstations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Below is an example manifest at
``/etc/puppetlabs/code/environments/simp/modules/site/manifests/virt.pp``
that would allow users to run ``libvirt`` virtual machines.

Importantly, note the ``libvirt::polkit`` class being called that sets the
users that are allowed to use ``libvirt`` from the command line.

.. code-block:: ruby

   # If you want users to be able to run VMs on their workstations
   # include a class like this.
   # If this is installed, VM creation and management is still limited by PolicyKit

   class site::virt {
     include 'libvirt::kvm'
     include 'libvirt::ksm'
     include 'swap'
     include 'network'

     #set up a local bridge on the network
     network::eth { "em1":
       bridge => 'br0',
       hwaddr => $facts['macaddress_em1']
     }

     network::eth { "br0":
       net_type => 'Bridge',
       hwaddr   => $facts['macaddress_em1'],
       require  => Network::Eth['em1']
     }

     #add virt-manager package
     package { 'virt-manager': ensure => 'latest' }

     # Create polkit policy to allow users in virsh users group to use libvirt
     class { 'libvirt::polkit':
       ensure => present,
       group  => 'virshusers',
       local  => true,
       active => true
     }

     #Create group and add users.
     group{ 'virshusers':
       members => ['user1','user2']
     }

   }

To set specific :term:`swappiness` values use :term:`Hiera` as follows:

.. code-block:: yaml

  # Settings for swap for creating/running virtual machines
  swap::high_swappiness: 80
  swap::max_swappiness: 100

Printer Setup
^^^^^^^^^^^^^

Below are example manifests for setting up a printing environment.

Setting up a Print Client
"""""""""""""""""""""""""

The following example sets up client-side printing and is expected to be
located at
``/etc/puppetlabs/code/environments/simp/modules/site/manifests/print/client.pp``.

.. code-block:: ruby

   class site::print::client inherits site::print::server {
     polkit::local_authority { 'print_support':
       identity           => ['unix_group:*'],
       action             => 'org.opensuse.cupskhelper.mechanism.*',
       section_name       => 'Allow all print management permissions',
       result_any         => 'yes',
       result_interactive => 'yes',
       result_active      => 'yes'
     }

     package { 'cups-pdf': ensure => 'latest' }
     package { 'cups-pk-helper': ensure => 'latest' }
     package { 'system-config-printer': ensure => 'present' }
   }


Setting up a Print Server
"""""""""""""""""""""""""

The following example sets up a server-side printing and is expected to be
located at
``/etc/puppetlabs/code/environments/simp/modules/site/manifests/print/server.pp``.

.. code-block:: ruby

   class site::print::server {

     # Note, this is *not* set up for being a central print server.
     # You'll need to add the appropriate IPTables rules for that to work.
     package { 'cups': ensure => 'latest' }

     service { 'cups':
       enable     => 'true',
       ensure     => 'running',
       hasrestart => 'true',
       hasstatus  => 'true',
       require    => Package['cups']
     }
   }

VNC Setup
---------

:term:`Virtual Network Computing` (VNC) can be enabled to provide remote GUI
access to systems.

VNC Standard Setup
^^^^^^^^^^^^^^^^^^

.. NOTE::

   You must have the ``pupmod-simp-vnc`` RPM installed to use VNC on your
   system!

To enable remote access via VNC on the system, include ``vnc::server``
in Hiera for the node.

The default VNC setup that comes with SIMP can only be used over SSH and
includes three default settings:

+---------------+------------------------------------+
|Setting Type   |Setting Details                     |
+===============+====================================+
|Standard       | Port: 5901                         |
|               |                                    |
|               | Resolution: 1024x768@16            |
+---------------+------------------------------------+
|Low Resolution | Port: 5902                         |
|               |                                    |
|               | Resolution: 800x600@16             |
+---------------+------------------------------------+
|High Resolution| Port: 5903                         |
|               |                                    |
|               | Resolution: 1280x1024@16           |
+---------------+------------------------------------+

Table: VNC Default Settings

To connect to any of these settings, SSH into the system running the VNC
server and provide a tunnel to ``127.0.0.1:<VNC Port>``. Refer to the SSH
client's documentation for specific instructions.

To set up additional VNC port settings, refer to the code in
``/etc/puppetlabs/code/environments/simp/modules/vnc/manifests/server.pp``
for examples.

.. IMPORTANT::

   Multiple users can log on to the same system at the same time with no
   adverse effects; however, none of these sessions are persistent.

   To maintain a persistent VNC session, use the ``vncserver`` application on
   the remote host. Type ``man vncserver`` to reference the manual for
   additional details.

VNC Through a Proxy
^^^^^^^^^^^^^^^^^^^

The section describes the process to VNC through a proxy. This setup
provides the user with a persistent VNC session.

.. IMPORTANT::

   In order for this setup to work, the system must have a VNC server
   (``vserver.your.domain``), a VNC client (``vclnt.your.domain``), and a proxy
   (``proxy.your.domain``). A ``vuser`` account must also be set up as the
   account being used for the VNC. The ``vuser`` is a common user that has
   access to the server, client, and proxy.

Modify Puppet
"""""""""""""

If definitions for the machines involved in the VNC do not already exist
in Hiera, create an ``/etc/puppetlabs/code/environments/simp/hieradata/hosts/vserv.your.domain.yaml``
file. In the client hosts file, modify or create the entries shown in the
examples below. These additional modules will allow the ``vserv`` system to act
as a VNC server and the ``vclnt`` system to act as a client.

VNC Server node

.. code-block:: yaml

   # vserv.your.domain.yaml
   classes:
     - 'gnome'
     - 'mozilla::firefox'
     - 'vnc::server'


VNC client node

.. code-block:: yaml

   # vclnt.your.domain.yaml
   classes:
     - 'gnome'
     - 'mozilla::firefox'
     - 'vnc::client'


Run the Server
""""""""""""""

As ``vuser`` on ``vserv.your.domain``, type ``vncserver``.

The output should mirror the following:

    New 'vserv.your.domain:<Port Number> (vuser)' desktop is vserv.your.domain:<Port Number>

Starting applications specified in ``/home/vuser/.vnc/xstartup`` Log file
is ``/home/vuser/.vnc/vserv.your.domain:<Port Number>.log``

.. NOTE::

   Remember the port number; it will be needed to set up an SSH tunnel.

Set up an SSH Tunnel
""""""""""""""""""""

Set up a tunnel from the client (vclnt), through the proxy server
(proxy), to the server (vserv). The table below lists the steps to set
up the tunnel.


#. On the workstation, type ``ssh -l vuser -L 590***<Port Number>*:localhost:590***<Port Number>***proxy.your.domain**``

  .. NOTE::

     This command takes the user to the proxy.

#. On the proxy, type ``ssh -l vuser -L 590***<Port Number>*:localhost:590***<Port Number>***vserv.your.domain**``

  .. NOTE::

     This command takes the user to the VNC server.

Table: Set Up SSH Tunnel Procedure

.. NOTE::

   The port number in 590\ *<Port Number>* is the same port number as
   previously described. For example, if the *<Port Number>* was 6, then all
   references below to 590\ *<Port Number>* become 5906.


Set Up Clients
""""""""""""""

On ``vclnt.your.domain``, type ``vncviewer localhost:590\ ***<Port
Number>***`` to open the Remote Desktop viewer.

Troubleshooting VNC Issues
^^^^^^^^^^^^^^^^^^^^^^^^^^

If nothing appears in the terminal window, the :term:`X Windows` may have crashed. To
determine if this is the case, type ``ps -ef | grep XKeepsCrashing``

If any matches result, stop the process associated with the command and
try to restart ``vncviewer`` on ``vclnt.your.domain``.
