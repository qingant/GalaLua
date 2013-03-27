# -*- coding: utf-8 -*-

GROUP_HEAD=u"""
            <dt onclick="show_manage('grp%(index)dbody')">%(name)s</dt>
            <ul id="buttons" class="clr">
              <li id="refresh"><a href="/manage/admin/look/">&amp;&nbsp;</a></li>
              <li id="restart_all"><a href="javascript:sendCommand(%(index)d,'','restart')">&amp;&nbsp;</a></li>
              <li id="stop_all"><a href="javascript:sendCommand(%(index)d,'','stop')">&amp;&nb</a></li>
            </ul>

            <dd id="grp%(index)dbody">
"""

GROUP_TAIL=u"""
            </dd>
"""

GROUP_BODY_HEAD=u"""
              <dl>
"""

GROUP_MON_HEAD=u"""
                <dt onclick="show_manage('%(group)dmonbody')">%(name)s</dt>
                <dd id="%(group)dmonbody">
"""

GROUP_MON_TAIL=u"""
                </dd>
"""

GROUP_SER_HEAD=u"""
                <dt onclick="show_manage('%(group)dserbody')">%(name)s</dt>
                <dd id="%(group)dserbody">
                  <dl>
"""
GROUP_SER_TAIL=u"""
                  </dl>
                </dd>
"""

GROUP_BODY_TAIL=u"""
            </dd>
"""

TABLE_TITLE=u"""
                    <dt id="webtools" onclick="show_manage('%(body)s')">%(name)s</dt>
"""

TABLE_HEAD=u"""
                  <table cellspacing="0" id="%(body)s">
                    <thead>
                      <tr>
                        <th class="name">Name</th>
                        <th class="desc">Description</th>
                        <th class="state">State</th>
                        <th class="action">Action</th>
                      </tr>
                    </thead>

                    <tbody>
"""

TABLE_TAIL=u"""
                    </tbody>
                  </table>
"""

#id="grpXtoken"
#class="shade"/""
#statustype
PRO_HEAD=u"""
                      <tr id="%(id)s" class="%(class)s">
"""
PRO_CTX=u"""
                        <td><a href="tail.html?processname=blog3" target="_blank">%(name)s</a></td>
                        <td><span>%(description)s</span></td>
                        <td class="status"><span class="%(spawnerr)s">%(statename)s</span></td>
"""

PRO_STOP_ACTION=u"""
                        <td class="action">
                          <ul>
                            <li>
                              <a href="javascript:sendCommand(%(group)d,'%(token)s','start')" name="Start">Start</a>
                            </li>
                            <li>
                              <a class="hidden" href="#" name="#">Action</a>
                            </li>
                            <li>
                              <a href="index.html?processname=blog3&amp;action=clearlog" name="Clear Log">Clear Log</a>
                            </li>
                            <li>
                              <a href="logtail/blog3" name="Tail -f" target="_blank">Tail -f</a>
                            </li>
                          </ul>
                        </td>
"""

PRO_START_ACTION=u"""
                        <td class="action">
                          <ul>
                            <li>
                              <a href="javascript:sendCommand(%(group)d,'%(token)s','restart')" name="Restart">Restart</a>
                            </li>
                          <li>
                              <a href="javascript:sendCommand(%(group)d,'%(token)s','stop')" name="Stop">Stop</a>
                            </li>
                          <li>
                              <a href="index.html?processname=blog&amp;action=clearlog" name="Clear Log">Clear Log</a>
                            </li>
                          <li>
                              <a href="logtail/blog" name="Tail -f" target="_blank">Tail -f</a>
                            </li>
                          </ul>
                        </td>
"""


PRO_TAIL=u"""
                      </tr>
"""

MON_START_ACTION=u"""
                        <td class="action">
                          <ul>
                            <li>
                              <a href="javascript:sendCommand(%(group)d,'%(token)s','restart')" name="Restart">Restart</a>
                            </li>
                          <li>
                              <a href="javascript:sendCommand(%(group)d,'%(token)s','stop')" name="Stop">shutdown</a>
                            </li>
                          <li>
                              <a href="index.html?processname=blog&amp;action=clearlog" name="Clear Log">Clear Log</a>
                            </li>
                          <li>
                              <a href="logtail/blog" name="Tail -f" target="_blank">Tail -f</a>
                            </li>
                          </ul>
                        </td>
"""

MON_STOP_ACTION=u"""
                        <td class="action">
                          <ul>
                            <li>
                              <a href="javascript:sendCommand(%(group)d,%(token)s','start')" name="Start">Start</a>
                            </li>
                            <li>
                              <a class="hidden" href="#" name="#">Action</a>
                            </li>
                            <li>
                              <a href="index.html?processname=blog3&amp;action=clearlog" name="Clear Log">Clear Log</a>
                            </li>
                            <li>
                              <a href="logtail/blog3" name="Tail -f" target="_blank">Tail -f</a>
                            </li>
                          </ul>
                        </td>
"""
