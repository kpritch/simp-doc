.. _gsg-advanced-configuration:

Advanced Configuration
----------------------
The goal of ``simp config`` is to allow the user to quickly configure the
SIMP server with minimal user input/operations.  To that end ``simp config``
sets installation variables based on information gathered from the user,
existing system settings, and SIMP security requirements. It then
applies the smallest subset of these system settings that is required to
bootstrap the system with Puppet.  Both the installation variables and
their application via ``simp config`` are described in subsections that
follow.

.. _List of Installation Variables:

Installation Variables
^^^^^^^^^^^^^^^^^^^^^^
This section describes the installation variables set by ``simp config``.
Although the table that follows lists all possible installation variables,
the user will not be prompted for all of them, nor will all of them
appear in the configuration files generated by ``simp config``.  Some
of these variables will be automatically set based on other installation
variables, system settings, or SIMP security requirements.  Others will
be omitted because either they are unnecessary for a particular site
configuration, or their defaults are appropriate. Also, please note
that variables beginning with 'cli::' are only used internally by
``simp config``, itself.  The 'cli::' variables are written to
``simp_conf.yaml``, but not persisted to any Puppet hieradata files.

.. IMPORTANT::

   - Not all the settings listed below can be preset in a
     configuration file input to ``simp config``, via either
     ``-a <Config File>`` or ``-A <Config File>``.  Only settings
     for which you would be prompted, if you ran ``simp config``
     interactively, can be preset.  All other settings will be
     automatically determined by ``simp config``, disregarding your
     input.

   - ``simp config`` behaves differently (asks different questions,
     automatically determines different settings) depending on the SIMP
     installation type.  This is because it can safely assume certain
     server setup has been done, only if SIMP has been installed from
     the SIMP-provided ISO. For example, consider a ``simp`` local
     user.  When SIMP is installed from ISO, ``simp config`` can safely
     assume that this user is the backup user installed by the ISO
     to prevent server lockout.  As such, ``su`` and ``ssh`` privileges
     for the ``simp`` user should be allowed.  For non-ISO installs,
     however, it would not be prudent for ``simp config`` to grant
     just any ``simp`` user both ``su`` and ``ssh`` privileges.

   - ``simp config`` detects that SIMP has been installed from a
     SIMP-provided ISO by the presence of
     ``/etc/yum.repos.d/simp_filesystem.repo``.

+--------------------------------+-------------------------------------------+
| Variable                       | Description                               |
+================================+===========================================+
| cli::is_ldap_server            | Whether the SIMP server will also be      |
|                                | the LDAP server.                          |
+--------------------------------+-------------------------------------------+
| cli::network::dhcp             | Whether to use DHCP for the network;      |
|                                | *dhcp* to enable DHCP, *static* otherwise |
+--------------------------------+-------------------------------------------+
| cli::network::gateway          | Default gateway                           |
+--------------------------------+-------------------------------------------+
| cli::network::hostname         | FQDN of server                            |
+--------------------------------+-------------------------------------------+
| cli::network::interface        | Network interface to use                  |
+--------------------------------+-------------------------------------------+
| cli::network::ipaddress        | IP address of server                      |
+--------------------------------+-------------------------------------------+
| cli::network::netmask          | Netmask of the system                     |
+--------------------------------+-------------------------------------------+
| cli::network::set_up_nic       | Whether to set up the network interface;  |
|                                | *true* or *false*                         |
+--------------------------------+-------------------------------------------+
| cli::set_grub_password         | Whether to set a GRUB password on the     |
|                                | server; *true* or *false*                 |
+--------------------------------+-------------------------------------------+
| cli::set_production_to_simp    | Whether to set default Puppet environment |
|                                | to 'simp'; *true* or *false*              |
+--------------------------------+-------------------------------------------+
| cli::simp::scenario            | SIMP scenario; *simp* = full SIMP system, |
|                                | *simp_lite* = SIMP system with some       |
|                                | security features disabled for clients,   |
|                                | *poss* = SIMP system with all security    |
|                                | features disabled for clients.            |
+--------------------------------+-------------------------------------------+
| cli::\                         | Whether to configure SIMP nodes to use    |
| use_internet_simp_yum_repos    | internet SIMP and SIMP dependency YUM     |
|                                | repositories.                             |
+--------------------------------+-------------------------------------------+
| grub::password                 | GRUB password hash                        |
+--------------------------------+-------------------------------------------+
| puppetdb::master::config::\    | Port used by the puppet database          |
| puppetdb_port                  |                                           |
+--------------------------------+-------------------------------------------+
| puppetdb::master::config::\    | DNS name or IP of puppet database server  |
| puppetdb_server                |                                           |
+--------------------------------+-------------------------------------------+
| simp_openldap::server::conf::\ | LDAP Root password hash                   |
| rootpw                         |                                           |
+--------------------------------+-------------------------------------------+
| simp_options::\dns::search     | Search domain for DNS                     |
+--------------------------------+-------------------------------------------+
| simp_options::\dns::servers    | List of DNS servers for the managed hosts |
+--------------------------------+-------------------------------------------+
| simp_options::fips             | Enable FIPS-140-2 compliance; *true*      |
|                                | or *false*; value automatically set to    |
|                                | detected system FIPS status               |
+--------------------------------+-------------------------------------------+
| simp_options::\ldap            | Whether to use LDAP; *true* or *false*    |
+--------------------------------+-------------------------------------------+
| simp_options::\ldap::base_dn   | LDAP Server Base Distinguished Name       |
+--------------------------------+-------------------------------------------+
| simp_options::\ldap::bind_dn   | LDAP Bind Distinguished Name              |
+--------------------------------+-------------------------------------------+
| simp_options::\ldap::bind_hash | LDAP Bind password hash                   |
+--------------------------------+-------------------------------------------+
| simp_options::\ldap::bind_pw   | LDAP Bind password                        |
+--------------------------------+-------------------------------------------+
| simp_options::\ldap::master    | LDAP master URI                           |
+--------------------------------+-------------------------------------------+
| simp_options::\ldap::sync_dn   | LDAP Sync Distinguished Name              |
+--------------------------------+-------------------------------------------+
| simp_options::\ldap::sync_hash | LDAP Sync password hash                   |
+--------------------------------+-------------------------------------------+
| simp_options::\ldap::sync_pw   | LDAP Sync password                        |
+--------------------------------+-------------------------------------------+
| simp_options::\ldap::uri       | List of LDAP server URIs                  |
+--------------------------------+-------------------------------------------+
| simp_options::ntpd::servers    | NTP servers                               |
+--------------------------------+-------------------------------------------+
| simp_options::puppet::ca       | FQDN of Puppet Certificate Authority (CA) |
+--------------------------------+-------------------------------------------+
| simp_options::puppet::ca_port  | Port Puppet CA will listen on             |
+--------------------------------+-------------------------------------------+
| simp_options::puppet::server   | FQDN of the puppet server                 |
+--------------------------------+-------------------------------------------+
| simp_options::sssd             | Whether to use SSSD                       |
+--------------------------------+-------------------------------------------+
| simp_options::syslog::\        | IP addresses of failover log servers      |
| failover_log_servers           |                                           |
+--------------------------------+-------------------------------------------+
| simp_options::syslog::\        | IP addresses of primary log servers       |
| log_servers                    |                                           |
+--------------------------------+-------------------------------------------+
| simp_options::trusted_nets     | Subnet used for clients managed by the    |
|                                | puppet server                             |
+--------------------------------+-------------------------------------------+
| simp::runlevel                 | Default system run level; 1-5             |
+--------------------------------+-------------------------------------------+
| simp::server::allow_simp_user  | Whether to allow local 'simp' user su and |
|                                | ssh privileges.                           |
+--------------------------------+-------------------------------------------+
| simp::yum::repo::\             | Whether to enable the SIMP-managed, OS    |
| local_os_updates::enable_repo  | Update YUM repository that the SIMP ISO   |
|                                | installs on the SIMP server.              |
+--------------------------------+-------------------------------------------+
| simp::yum::repo::\             | YUM server(s) for SIMP-managed, OS Update |
| local_os_updates::servers      | packages                                  |
+--------------------------------+-------------------------------------------+
| simp::yum::repo::\             | Whether to enable the SIMP-managed, SIMP  |
| local_simp::enable_repo        | and SIMP dependency YUM repository that   |
|                                | the SIMP ISO installs on the SIMP server. |
+--------------------------------+-------------------------------------------+
| simp::yum::repo::\             | YUM server(s) for SIMP-managed, SIMP and  |
| local_simp::servers            | SIMP dependency packages                  |
+--------------------------------+-------------------------------------------+
| sssd::domains                  | List of SSSD domains                      |
+--------------------------------+-------------------------------------------+
| svckill::mode                  | Strategy svckill should use when it       |
|                                | encounters undeclared services;           |
|                                | *enforcing* = shutdown and disable all    |
|                                | services not listed in your manifests or  |
|                                | the exclusion file *warning* = only       |
|                                | report what undeclared services should be |
|                                | shut down and disabled, without actually  |
|                                | making the changes to the system          |
+--------------------------------+-------------------------------------------+
| useradd::securetty             | A list of TTYs for which the root user    |
|                                | can login                                 |
+--------------------------------+-------------------------------------------+

.. _simp config Actions:

simp config Actions
^^^^^^^^^^^^^^^^^^^

In addition to creating the three configuration, YAML files, ``simp config``
performs a limited set of actions in order to prepare the system for
bootstrapping.  Although the table that follows lists all possible
``simp config`` actions, not all of these actions will apply for all site
configurations.

+---------------+--------------------------------------------------------------+
| Category      | Actions Performed                                            |
+===============+==============================================================+
| Certificates  | If no certificates for the host are found in                 |
|               | ``/var/simp/environments/simp/site_files/pki_files/``        |
|               | ``files/keydist``, ``simp config`` will use SIMP's FakeCA    |
|               | to generate interim host certificates.  These certificates,  |
|               | which are independent of the certificates managed by Puppet, |
|               | are required by SIMP and should be replaced by certificates  |
|               | from an official :term:`Certificate Authority`, as soon as   |
|               | is practical.                                                |
+---------------+--------------------------------------------------------------+
| Digest        | When the system is in :term:`FIPS` mode,                     |
| Algorithm for | ``simp config`` will set the Puppet digest algorithm to      |
| FIPS          | *sha256* to prevent any Puppet-related actions executed by   |
|               | ``simp config`` from using MD5 checksums. Note that this is  |
|               | **not** all that must be done to enable FIPS. The complete   |
|               | set of actions required to support FIPS is handled by        |
|               | ``simp bootstrap``.                                          |
+---------------+-----------------+--------------------------------------------+
| GRUB          |  When the user selects to set the GRUB password              |
|               |  ``simp config`` will set the password in the appropriate    |
|               |  grub configuration file, ``/etc/grub.conf`` or              |
|               |  ``/etc/grub2.cfg``.                                         |
+---------------+--------------------------------------------------------------+
| LDAP          | When the SIMP server is also an LDAP server, ``simp config`` |
|               |                                                              |
|               | - Adds ``simp::server::ldap`` to the SIMP server host YAML   |
|               |   file, which allows the SIMP server to act as a LDAP server |
|               | - Adds the hash of the user-supplied LDAP root password to   |
|               |   the SIMP server host YAML file as                          |
|               |   ``simp_openldap::server::conf::rootpw`` to the SIMP        |
+---------------+--------------------------------------------------------------+
| Lockout       | When the SIMP server is installed from ISO, the install      |
| Prevention    | creates a local *simp* user that the SIMP server configures  |
|               | to have both su and ssh privileges. (This user is provided   |
|               | to prevent server lockout, as, per security policy, SIMP by  |
|               | default disables logins via ssh for all users, including     |
|               | 'root'.) So, when SIMP is **not** installed from ISO,        |
|               | ``simp config`` does the following:                          |
|               |                                                              |
|               | - Warns the operator of this problem                         |
|               | - Writes a lock file containing details on how to rectify    |
|               |   the problem.  This lock file prevents ``simp bootstrap``   |
|               |   from running until the user manually fixes the problem.    |
|               | - Turns off the SIMP server configuration that allows        |
|               |   su and ssh privileges for an inapplicable *simp* user.     |
+---------------+--------------------------------------------------------------+
| Network       | - When the user selects to configure the network interface,  |
|               |   ``simp config`` uses Puppet to set the network interface   |
|               |   parameters in system networking files and to bring up the  |
|               |   interface.                                                 |
|               | - ``simp config`` sets the hostname.                         |
+---------------+--------------------------------------------------------------+
| Puppet        | - Copies SIMP modules installed via RPM in                   |
|               |   ``/usr/share/simp`` into the Puppet environments directory |
|               |   ``/etc/puppetlabs/code/environments`` if necessary.        |
|               | - When selected, sets the default Puppet environment to      |
|               |   'simp', backing up the existing 'production' environment,  |
|               |   if it exists.                                              |
|               | - Creates/updates ``/etc/puppetlabs/puppet/autosign.conf``.  |
|               | - Updates the following Puppet settings:                     |
|               |   ``digest_algorithm``, ``keylength``, ``server``,           |
|               |   ``ca_server``, ``ca_port``, and ``trusted_server_facts``.  |
|               | - Updates ``/etc/hosts`` to ensure a puppet server entry     |
|               |   exists.                                                    |
+---------------+--------------------------------------------------------------+
| SIMP Hiera &  | - Sets the ``$simp_scenario`` variable in the site.pp of the |
| Site Manifest |   'simp' environment to the user-selected scenario.          |
|               | - If a host YAML file for the SIMP server does not already   |
|               |   exist in                                                   |
|               |   ``/etc/puppetlabs/code/environments/simp/hieradata/hosts`` |
|               |   does not already exist, ``simp config`` will create one    |
|               |   from a SIMP template.                                      |
|               | - Updates the SIMP server host YAML file with appropriate    |
|               |   PuppetDB settings.                                         |
|               | - Creates YAML file containing global hieradata relevant to  |
|               |   both the SIMP server and SIMP clients in the 'simp',       |
|               |   environment, ``simp/hieradata/simp_config_settings.yaml``  |
+---------------+--------------------------------------------------------------+
| YUM           | - When the SIMP filesystem YUM repo from an ISO install      |
|               |   exists (``/etc/yum.repos.d/simp_filesystem.repo``),        |
|               |   ``simp config``                                            |
|               |                                                              |
|               |   - Configures SIMP server to act as a YUM server for the    |
|               |     on-server repo, by adding the ``simp::server::yum``      |
|               |     class to the SIMP server host YAML file.                 |
|               |   - Configures SIMP clients to use the SIMP server's YUM     |
|               |     repos by adding ``simp::yum::repo::local_os_updates``    |
|               |     and ``simp::yum::repo::local_simp`` classes to           |
|               |     ``simp_config_settings.yaml``.                           |
|               |   - Disables the use of the ``simp::yum::repo::local*``      |
|               |     repos in the SIMP server's host YAML file, as it is      |
|               |     already configured to use the more efficient, filesystem |
|               |     repo.                                                    |
|               |   - Updates the appropriate OS YUM Updates repository,       |
|               |     contained at ``/var/www/yum/OSTYPE/MAJORRELEASE/ARCH``.  |
|               |   - Disables any default CentOS repos.                       |
|               |                                                              |
|               | - When the SIMP filesystem YUM repo does not exist, but the  |
|               |   user wants to use internet repos ``simp config``           |
|               |                                                              |
|               |   - Enables internet SIMP server repos in the SIMP server    |
|               |     host YAML file by adding the                             |
|               |     ``simp::yum::repo::internet_simp_server`` class.         |
|               |   - Enables internet SIMP dependency repos for both SIMP     |
|               |     clients and in the SIMP server by adding the             |
|               |     ``simp::yum::repo::internet_simp_dependencies`` class    |
|               |     to ``simp_config_settings.yaml``.                        |
|               |                                                              |
|               | - When the SIMP filesystem YUM repo does not exist and the   |
|               |   user does not want to use internet repos,                  |
|               |   ``simp config``                                            |
|               |                                                              |
|               |   - Checks the configuration of the  SIMP server's YUM repos |
|               |     via ``repoquery``.  If this check fails, writes a lock   |
|               |     to prevent ``simp bootstrap`` from running until the     |
|               |     user manually fixes the issue.                           |
|               |   - Reminds the user to (manually) set up YUM repos for SIMP |
|               |     clients.                                                 |
+---------------+--------------------------------------------------------------+

.. todo simp bootstrap Actions
