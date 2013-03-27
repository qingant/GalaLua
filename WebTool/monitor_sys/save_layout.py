import afa_context as context
import web
import sqlite3 
import os
import json
layout_dir = os.path.join(os.getenv("HOME"), "var/layout/")
    
if not os.path.exists(layout_dir):
    os.mkdir(layout_dir)
os.chdir(layout_dir)

class current_layout:
    def GET(self):
        conn = sqlite3.connect(layout_dir+'layout_data.db')
        c = conn.cursor()
        c.execute('''CREATE TABLE  IF NOT EXISTS current_layout (user_name text, cpu_top real, cpu_left real, mem_top real, mem_left real, c_bloon int, m_bloon int)''')
        data = web.input();
        all_data = (float(data['c_top']), float(data['c_left']), float(data['m_top']), float(data['m_left']), int(data['c_bloon']), int(data['m_bloon']), data['user'])
        for r in c.execute("SELECT * FROM current_layout"):
            if r[0] == data['user']:
                c.execute("UPDATE current_layout SET cpu_top = ?, cpu_left = ?, mem_top = ?, mem_left = ?, c_bloon=?, m_bloon=? WHERE user_name = ?", all_data)
                print all_data
                conn.commit()
                conn.close()
                print data['user']
                return 0
                        
        c.execute("INSERT INTO current_layout VALUES('%s', '%f', '%f', '%f', '%f', '%d', '%d')" % (data['user'], float(data['c_top']), float(data['c_left']), float(data['m_top']), float(data['m_left']), int(data['c_bloon']), int(data['m_bloon'])))
        conn.commit()
        conn.close()
        return 0
        
class load_current_layout:
    def GET(self):
        layout_init = {'c_top':0, 'c_left':220, 'm_top':150, 'm_left':220, 'c_bloon':0, 'm_bloon':0}

        if not os.path.exists(layout_dir + "layout_data.db"):
            return layout_init
            
        conn = sqlite3.connect(layout_dir + "layout_data.db")
        c = conn.cursor()
        data = web.input()
        user_data = (data['user'],)
        
        
        for r in c.execute("SELECT * FROM current_layout WHERE user_name = ?", user_data):
            if r is not None:
                layout_data = r
                layout_return = {'c_top':layout_data[1], 'c_left':layout_data[2], 'm_top':layout_data[3], 'm_left':layout_data[4], 'c_bloon':layout_data[5], 'm_bloon':layout_data[6]}
                conn.close()
                print layout_return
                return json.dumps(layout_return)
        
        
        return json.dumps(layout_init)
   
