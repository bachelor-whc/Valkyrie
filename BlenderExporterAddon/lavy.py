bl_info = {
    "name": ".lavy exporter",
    "category": "Import-Export",
}

import bpy
import json
import bmesh
import ctypes
from bpy.props import StringProperty, BoolProperty
from bpy_extras.io_utils import ExportHelper

class LavyExporter(bpy.types.Operator, ExportHelper):
    bl_idname = "export_mesh.lavy"
    bl_label = "Export .lavy"
    filename_ext = ".lavy"
    filter_glob = StringProperty(default="*.lavy", options={'HIDDEN'})
	
    def execute(self, context):
        scene = bpy.context.scene
		
        if len(bpy.context.selected_objects)>0:
            exporting_obj = bpy.context.selected_objects[0]
            mesh = exporting_obj.data
            b_mesh = bmesh.new()
            b_mesh.from_mesh(mesh)
            bmesh.ops.triangulate(b_mesh, faces=b_mesh.faces)
            b_mesh.verts.index_update()
            b_mesh.verts.ensure_lookup_table()
            b_mesh.faces.index_update()
            b_mesh.faces.ensure_lookup_table()

            export_json = open(self.filepath + ".json", "w")
            export_bin = open(self.filepath + ".bin", "wb")

            uv_layer = b_mesh.loops.layers.uv.active

            offset = 0

            for vert in b_mesh.verts:
                loop = vert.link_loops[0]
                for pos in vert.co:
                    export_bin.write(ctypes.c_float(pos))
                for normal in vert.normal:
                    export_bin.write(ctypes.c_float(normal))
                for uv in loop[uv_layer].uv:
                    export_bin.write(ctypes.c_float(uv))
                offset += 32
            
            vertices_byte_length = offset

            for face in b_mesh.faces:
                for vert in face.verts:
                    i = ctypes.c_uint32(vert.index)
                    export_bin.write(i)
                    offset += 4
            
            indices_byte_length = offset - vertices_byte_length

            j_data = json.dumps(
                {
                    'vertices': {
                        'byteLength': vertices_byte_length,
                        'byteOffset': 0
                    },
                    'indices': {
                        'byteLength': indices_byte_length,
                        'byteOffset': vertices_byte_length
                    }
                }
            )

            export_json.write(j_data)

            export_json.close()
            export_bin.close()
            b_mesh.free()
        return {'FINISHED'}

def menu_export(self, context):
    self.layout.operator(LavyExporter.bl_idname, text="Lavy")

def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_export)

def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_export)

if __name__ == "__main__":
    register()